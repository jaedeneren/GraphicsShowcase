R"(
#version 330 core

layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 u_view;
uniform mat4 u_proj;

void main()
{
    TexCoords = aPos;

    // Remove translation from view matrix so skybox stays centered on camera
    mat4 viewNoTranslation = mat4(mat3(u_view));
    vec4 pos = u_proj * viewNoTranslation * vec4(aPos, 1.0);

    // Set z = w so depth is always 1.0 (skybox renders behind everything)
    gl_Position = pos.xyww;
}
)"