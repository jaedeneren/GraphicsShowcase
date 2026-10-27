R"(
#version 330 core

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 VertexTexCoord;

uniform mat4 M;       // Model matrix - set per object by framework
uniform mat4 u_view;  // View matrix  - set once in InitCallback
uniform mat4 u_proj;  // Proj matrix  - set once in InitCallback

out vec3 Position;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    mat4 MV       = u_view * M;
    mat3 NormalMat = transpose(inverse(mat3(MV)));

    Position    = vec3(MV * vec4(VertexPosition, 1.0));
    Normal      = normalize(NormalMat * VertexNormal);
    TexCoord    = VertexTexCoord;
    gl_Position = u_proj * MV * vec4(VertexPosition, 1.0);
}
)"