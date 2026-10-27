R"(
#version 460 core

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vUV;
in vec4 vProjTexCoord;

out vec4 FragColor;

uniform float iTime;

vec3 hash33(vec3 p3)
{
    vec3 MOD3 = vec3(0.1031, 0.11369, 0.13787);
    p3 = fract(p3 * MOD3);
    p3 += dot(p3, p3.yxz + 19.19);
    return -1.0 + 2.0 * fract(vec3(
        (p3.x + p3.y) * p3.z,
        (p3.x + p3.z) * p3.y,
        (p3.y + p3.z) * p3.x));
}

float simplex_noise(vec3 p)
{
    const float K1 = 0.333333333;
    const float K2 = 0.166666667;

    vec3 i = floor(p + (p.x + p.y + p.z) * K1);
    vec3 d0 = p - (i - (i.x + i.y + i.z) * K2);

    vec3 e = step(vec3(0.0), d0 - d0.yzx);
    vec3 i1 = e * (1.0 - e.zxy);
    vec3 i2 = 1.0 - e.zxy * (1.0 - e);

    vec3 d1 = d0 - (i1 - 1.0 * K2);
    vec3 d2 = d0 - (i2 - 2.0 * K2);
    vec3 d3 = d0 - (1.0 - 3.0 * K2);

    vec4 h = max(0.6 - vec4(
        dot(d0, d0),
        dot(d1, d1),
        dot(d2, d2),
        dot(d3, d3)), 0.0);
    vec4 n = h * h * h * h * vec4(
        dot(d0, hash33(i)),
        dot(d1, hash33(i + i1)),
        dot(d2, hash33(i + i2)),
        dot(d3, hash33(i + 1.0)));

    return dot(vec4(31.316), n);
}

float renderPortal(vec2 uv)
{
    float radial = length(uv);
    float side = smoothstep(1.0, 0.55, radial);
    float center = smoothstep(0.16, 0.0, radial);
    vec3 rd = vec3(uv, 0.0);

    float t = pow(iTime + 0.5, 5.0) * 0.001;
    float rayLen = max(length(rd * t + rd), 0.0001);
    float n2 = simplex_noise((rd * t + t) * (1.0 / rayLen));

    if (iTime > 1.5)
        n2 = simplex_noise((rd * t + t) * (1.0 / rayLen) + (iTime - 1.5));

    float flareDenom = max(length(rd * max(length(rd), 0.0001) * n2), 0.0001);
    float flare = smoothstep(0.0, 1.0, 0.002 / flareDenom) * side;
    flare -= center * clamp((iTime - 1.5) * 10.0, 0.0, 5.0);

    return max(flare, 0.0);
}

void main()
{
    if (vProjTexCoord.w <= 0.0)
        discard;

    vec2 projUV = vProjTexCoord.xy / vProjTexCoord.w;
    if (projUV.x <= 0.0 || projUV.x >= 1.0 || projUV.y <= 0.0 || projUV.y >= 1.0)
        discard;

    vec2 uv = projUV - 0.5;
    uv *= 2.0;

    float portalMask = 1.0 - smoothstep(0.7, 1.0, length(uv));
    float flare = renderPortal(uv) * portalMask;

    if (flare < 0.001)
        discard;

    vec3 color = vec3(0.102, 0.5, 1.0) * 2.0 * flare;
    color += vec3(0.75, 0.95, 1.0) * pow(max(portalMask, 0.0), 4.0) * flare * 0.2;

    FragColor = vec4(min(color, vec3(1.0)), 1.0);
}
)"
