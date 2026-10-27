R"(
#version 330 core

layout(location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 M;
uniform mat4 u_view;
uniform mat4 u_proj;

void main()
{
    // Keep the cube fixed in world space so camera translation is visible.
    TexCoords = aPos;
    gl_Position = u_proj * u_view * M * vec4(aPos, 1.0);
}
)"
