R"(
#version 460 core

layout(location = 0) out vec4 FragColor;

layout(binding = 0) uniform sampler2D ColorTex;
layout(binding = 1) uniform sampler2D NormalMapTex;
layout(binding = 2) uniform sampler2D GLightDirTex;
layout(binding = 3) uniform sampler2D GViewDirTex;
layout(binding = 4) uniform sampler2D GSurfaceTex;

uniform vec2 iResolution;
uniform vec3 u_lightAmbient;
uniform vec3 u_lightDiffuse;
uniform vec3 u_lightSpecular;
uniform float u_shininess = 48.0f;
uniform int u_renderMode = 1;

vec3 toneMap(vec3 color)
{
    color = color / (color + vec3(1.0));
    return pow(color, vec3(1.0 / 2.2));
}

void main()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);

    vec3 lightDir = texelFetch(GLightDirTex, pix, 0).rgb;
    vec3 viewDir = texelFetch(GViewDirTex, pix, 0).rgb;
    vec3 surfaceData = texelFetch(GSurfaceTex, pix, 0).rgb;
    vec2 texCoord = surfaceData.xy;
    float specStrength = surfaceData.z;

    if (specStrength < 0.001)
    {
        vec2 uv = gl_FragCoord.xy / max(iResolution, vec2(1.0));
        vec3 bg = mix(vec3(0.02, 0.02, 0.03), vec3(0.05, 0.05, 0.07), uv.y);
        FragColor = vec4(bg, 1.0);
        return;
    }

    vec3 tangentNormal = texture(NormalMapTex, texCoord).xyz * 2.0 - 1.0;
    vec3 albedo = pow(texture(ColorTex, texCoord).rgb, vec3(2.2));
    vec3 N = normalize(tangentNormal);
    vec3 L = normalize(lightDir);
    vec3 V = normalize(viewDir);

    if (u_renderMode == 2)
    {
        FragColor = vec4(L * 0.5 + 0.5, 1.0);
        return;
    }

    if (u_renderMode == 3)
    {
        FragColor = vec4(V * 0.5 + 0.5, 1.0);
        return;
    }

    vec3 H = normalize(L + V);

    float diff = max(dot(N, L), 0.0);
    float spec = 0.0;
    if (diff > 0.0)
        spec = pow(max(dot(N, H), 0.0), u_shininess) * specStrength;

    vec3 litColor =
        albedo * u_lightAmbient +
        albedo * u_lightDiffuse * diff +
        u_lightSpecular * spec;

    FragColor = vec4(toneMap(litColor), 1.0);
}
)"
