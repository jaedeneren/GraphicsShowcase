R"(
#version 420

layout(location=0) out vec4 FragColor;

layout(binding=0) uniform sampler2D Texture0;
uniform int BlurEnabled = 1;

float Weight[5] = float[5](0.158435f, 0.148836f, 0.123389f, 0.0902733f, 0.0582848f);

void main()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);

    if (BlurEnabled == 0)
    {
        FragColor = texelFetch(Texture0, pix, 0);
        return;
    }

    vec4 sum = texelFetch(Texture0, pix, 0) * Weight[0];
    sum += texelFetchOffset(Texture0, pix, 0, ivec2(0,-4)) * Weight[4];
    sum += texelFetchOffset(Texture0, pix, 0, ivec2(0,-3)) * Weight[3];
    sum += texelFetchOffset(Texture0, pix, 0, ivec2(0,-2)) * Weight[2];
    sum += texelFetchOffset(Texture0, pix, 0, ivec2(0,-1)) * Weight[1];
    sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, 1)) * Weight[1];
    sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, 2)) * Weight[2];
    sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, 3)) * Weight[3];
    sum += texelFetchOffset(Texture0, pix, 0, ivec2(0, 4)) * Weight[4];

    FragColor = sum;
}
)"