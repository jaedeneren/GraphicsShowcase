R"(
#version 330 core

in vec3 vWorldPos;
in vec3 vWorldNrm;
in vec3 vObjPos;

out vec4 FragColor;

uniform vec3 u_camPos;
uniform float u_reflectionFactor;   // 0>1
uniform float u_eta;                // IOR ratio
uniform int u_isSkybox;             // 1 = skybox, 0 = object

// ------------------------------------------------------------
// Checkerboard pattern
// ------------------------------------------------------------
vec3 checker(vec2 uv)
{
    float scale = 6.0;
    vec2 p = floor(uv * scale);
    float c = mod(p.x + p.y, 2.0);
    return mix(vec3(0.15), vec3(0.85), c);
}

// ------------------------------------------------------------
// SKybox
// ------------------------------------------------------------
vec3 envColorFromDir(vec3 d)
{
    d = normalize(d);
    vec3 ad = abs(d);

    vec2 uv;

    // X faces
    if (ad.x >= ad.y && ad.x >= ad.z)
    {
        if (d.x > 0.0)  // +X
            uv = vec2(-d.z,  d.y) / ad.x;
        else            // -X
            uv = vec2( d.z,  d.y) / ad.x;

        uv = uv * 0.5 + 0.5;
        return checker(uv);
    }
    // Y faces -> IGNORE (no floor / ceiling)
    else if (ad.y >= ad.x && ad.y >= ad.z)
    {
        return vec3(0.0);
    }
    // Z faces
    else
    {
        if (d.z > 0.0)  // +Z
            uv = vec2( d.x,  d.y) / ad.z;
        else            // -Z
            uv = vec2(-d.x,  d.y) / ad.z;

        uv = uv * 0.5 + 0.5;
        return checker(uv);
    }
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
void main()
{
    // --------------------------------------------------------
    // SKYBOX
    // --------------------------------------------------------
    if (u_isSkybox == 1)
    {
        vec3 dir = normalize(vObjPos);
        vec3 col = envColorFromDir(dir);
        FragColor = vec4(col, 1.0);
        return;
    }

    // --------------------------------------------------------
    // REFLECTIVE / REFRACTIVE OBJECT (sphere)
    // --------------------------------------------------------
    vec3 N = normalize(vWorldNrm);
    vec3 V = normalize(vWorldPos - u_camPos);

    // Reflection
    vec3 R = reflect(V, N);
    vec3 reflCol = envColorFromDir(R);

    // Refraction
    vec3 refrCol = vec3(0.0);
    vec3 T = refract(V, N, u_eta);
    if (length(T) > 0.0)
        refrCol = envColorFromDir(T);

    vec3 color = mix(refrCol, reflCol, u_reflectionFactor);
    FragColor = vec4(color, 1.0);
}

)"
