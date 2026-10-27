R"(
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aTangent;

uniform mat4 M;
uniform mat4 u_view;
uniform mat4 u_proj;

out vec3 v_viewPos;
out vec2 v_texCoord;
out vec3 v_tangentView;
out vec3 v_bitangentView;
out vec3 v_normalView;

void main()
{
    mat4 modelView = u_view * M;
    vec4 viewPos = modelView * vec4(aPos, 1.0);

    mat3 normalMatrix = mat3(transpose(inverse(modelView)));
    vec3 normalView = normalize(normalMatrix * aNormal);
    vec3 tangentView = normalize(normalMatrix * aTangent.xyz);
    tangentView = normalize(tangentView - dot(tangentView, normalView) * normalView);
    vec3 bitangentView = normalize(cross(normalView, tangentView)) * aTangent.w;

    v_viewPos = viewPos.xyz;
    v_texCoord = aTexCoord;
    v_tangentView = tangentView;
    v_bitangentView = bitangentView;
    v_normalView = normalView;

    gl_Position = u_proj * viewPos;
}
)"
