R"(
#version 460 core

in vec3 v_viewPos;
in vec2 v_texCoord;
in vec3 v_tangentView;
in vec3 v_bitangentView;
in vec3 v_normalView;

layout(location = 1) out vec3 GLightDir;
layout(location = 2) out vec3 GViewDir;
layout(location = 3) out vec3 GSurface;

struct Material
{
    vec3 Ka;
    vec3 Kd;
    vec3 Ks;
    float shininess;
};

uniform Material material;
uniform vec3 u_lightPosView;

void main()
{
    mat3 tangentToView = mat3(
        normalize(v_tangentView),
        normalize(v_bitangentView),
        normalize(v_normalView)
    );
    mat3 viewToTangent = transpose(tangentToView);

    vec3 tangentLightDir = normalize(viewToTangent * normalize(u_lightPosView - v_viewPos));
    vec3 tangentViewDir = normalize(viewToTangent * normalize(-v_viewPos));
    float specStrength = clamp((material.Ks.r + material.Ks.g + material.Ks.b) / 3.0, 0.05, 1.0);

    GLightDir = tangentLightDir;
    GViewDir = tangentViewDir;
    GSurface = vec3(v_texCoord, specStrength);
}
)"
