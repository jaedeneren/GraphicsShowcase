R"(
#version 330 core

in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube skybox;
uniform float u_skyExposure;

void main()
{
    vec3 color = texture(skybox, TexCoords).rgb * u_skyExposure;
    FragColor = vec4(color, 1.0);
}
)"