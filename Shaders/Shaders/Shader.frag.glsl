R"(
#version 330 core

uniform int u_tab;
uniform vec2 iResolution;   // Viewport resolution (in pixels)
uniform float iTime;        // Shader playback time (in seconds)
uniform vec2 u_mouse;       // Mouse pixel coords (origin bottom-left)
uniform float u_btnY;      // in UV (0-1)
uniform float u_btnRadius; // in UV (0-1)
uniform float u_spacing;   // in UV (0-1)

layout(location=0) out vec4 FragColor;

//  helpers
vec4 checkerboardTexture(vec2 uv, int size)
{
    uv = uv * size;
    float color = mod(floor(uv.x) + floor(uv.y), 2.0) / 5.0; // x+y even:black  odd:white
    return vec4(color, color, color, 1.0);
}

float circleMaskNorm(vec2 uv, vec2 center, float r)
{
    float d = length(uv - center);
    return smoothstep(r, r - 0.01, d);
}

vec3 lightSpot(vec2 uv, vec2 center, float radius, float time)
{
    float dist = length(uv - center);
    float intensity = 1.0 - smoothstep(0.0, radius, dist);
    float r = 0.5 + 0.5 * sin(time + 0.0);
    float g = 0.5 + 0.5 * sin(time + 2.0);
    float b = 0.5 + 0.5 * sin(time + 4.0);
    return vec3(r, g, b) * intensity;
}

vec3 staticLightSpotPixels(vec2 centerPx, float radiusPx, float strength)
{
    vec2 fragPx = gl_FragCoord.xy;
    float dist = length(fragPx - centerPx);
    float intensity = 1.0 - smoothstep(0.0, radiusPx, dist);
    return vec3(1.0) * intensity * strength;
}







void main()
{
    vec2 fragPx = gl_FragCoord.xy;
    vec2 uv = fragPx / iResolution.xy;

    // base content by tab
    vec3 content = vec3(0.0);

    if (u_tab == 0)
    {
        vec2 speed = vec2(0.1, 0.1);
        vec2 moved = uv + speed * iTime;
        content = checkerboardTexture(moved, 5).rgb;
    }
    else if (u_tab == 1)
    {
        content = vec3(0.0);
        content += lightSpot(uv, vec2(0.0, 0.0), 0.5, iTime);
        content += lightSpot(uv, vec2(1.0, 1.0), 0.5, iTime);
    }
    else if (u_tab == 2)
    {
        content = checkerboardTexture(uv, 10).rgb;
        vec2 mousePx = clamp(u_mouse, vec2(0.0), iResolution);
        content += staticLightSpotPixels(mousePx, 50.0, 1.6);
    }

    //buttons

    vec3 finalColor = content;


    for (int i = 0; i < 3; i++) {
        // Center of button in pixel coordinates
        float centerX = iResolution.x * 0.5 + float(i - 1) * u_spacing;
        float centerY = u_btnY; 
        vec2 btnCenterPx = vec2(centerX, centerY);

        // Distance from fragment to button center in pixels
        float distPx = length(gl_FragCoord.xy - btnCenterPx);
        float mask = smoothstep(u_btnRadius, u_btnRadius - 1.5, distPx);
        
        // Color logic
        vec3 btnCol = (u_tab == i) ? vec3(1.0) : vec3(0.4);
        float alpha = (u_tab == i) ? 0.8 : 0.4;
        
        finalColor = mix(finalColor, btnCol, mask * alpha);
    }

    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}
)"