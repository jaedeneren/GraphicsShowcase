R"(
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 M;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform mat4 u_projectorMatrix;

out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec2 vUV;
out vec4 vProjTexCoord;

void main()
{
    vec4 worldPos = M * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;
    vWorldNormal = mat3(transpose(inverse(M))) * aNormal;
    vUV = aUV;
    vProjTexCoord = u_projectorMatrix * worldPos;

    gl_Position = u_proj * u_view * worldPos;
}
)"
