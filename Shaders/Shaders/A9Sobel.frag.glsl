R"(
#version 420

layout(location=0) out vec4 FragColor;

layout(binding=2) uniform sampler2D Texture2;

uniform int EdgeEnabled = 1;
uniform float EdgeThreshold = 0.05f;

float luminance(vec3 color)
{
    return dot(vec3(0.2126f, 0.7152f, 0.0722f), color);
}

void main()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);

    if (EdgeEnabled == 0)
    {
        FragColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
        return;
    }

    float s00 = luminance(texelFetchOffset(Texture2, pix, 0, ivec2(-1, 1)).rgb);
    float s10 = luminance(texelFetchOffset(Texture2, pix, 0, ivec2(-1, 0)).rgb);
    float s20 = luminance(texelFetchOffset(Texture2, pix, 0, ivec2(-1,-1)).rgb);

    float s01 = luminance(texelFetchOffset(Texture2, pix, 0, ivec2( 0, 1)).rgb);
    float s21 = luminance(texelFetchOffset(Texture2, pix, 0, ivec2( 0,-1)).rgb);

    float s02 = luminance(texelFetchOffset(Texture2, pix, 0, ivec2( 1, 1)).rgb);
    float s12 = luminance(texelFetchOffset(Texture2, pix, 0, ivec2( 1, 0)).rgb);
    float s22 = luminance(texelFetchOffset(Texture2, pix, 0, ivec2( 1,-1)).rgb);

    float sx = s00 + 2.0f * s10 + s20 - (s02 + 2.0f * s12 + s22);
    float sy = s00 + 2.0f * s01 + s02 - (s20 + 2.0f * s21 + s22);

    float g = sx * sx + sy * sy;

    FragColor = (g > EdgeThreshold)
        ? vec4(1.0f)
        : vec4(0.0f, 0.0f, 0.0f, 1.0f);
}
)"