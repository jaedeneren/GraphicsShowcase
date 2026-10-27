R"(
#version 420

layout(location=0) out vec4 FragColor;

in vec3 v_pos;
in vec3 v_normal;

struct LightInfo
{
    vec3 position;
    vec3 La;
    vec3 Ld;
    vec3 Ls;
};

struct MaterialInfo
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
    float effect;
};

uniform LightInfo light[1];
uniform MaterialInfo material;
uniform mat4 u_view;

void main()
{
    vec3 N = normalize(v_normal);
    vec3 lightPosView = vec3(u_view * vec4(light[0].position, 1.0));
    vec3 L = normalize(lightPosView - v_pos);

    float ndotl = max(dot(N, L), 0.0);

    // smoother toon shading
    float tone = 0.45f;
    tone += 0.25f * smoothstep(0.15f, 0.30f, ndotl);
    tone += 0.20f * smoothstep(0.45f, 0.65f, ndotl);
    tone += 0.10f * smoothstep(0.75f, 0.90f, ndotl);

    vec3 baseColor;
    vec3 ambientColor;

    if (material.effect > 2.5f)
    {
        baseColor    = vec3(0.40f, 0.65f, 1.00f); // plane
        ambientColor = vec3(0.10f, 0.18f, 0.35f);
    }
    else
    {
        baseColor    = vec3(0.82f, 0.47f, 0.22f); // teapot
        ambientColor = vec3(0.18f, 0.08f, 0.03f);
    }

    vec3 color = ambientColor + baseColor * tone * 0.85f;
    FragColor = vec4(color, 1.0f);
}
)"