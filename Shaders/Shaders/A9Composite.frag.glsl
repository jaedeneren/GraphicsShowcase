R"(
#version 420

layout(location=0) out vec4 FragColor;

layout(binding=0) uniform sampler2D PhongTex;
layout(binding=2) uniform sampler2D BlurTex;
layout(binding=3) uniform sampler2D EdgeTex;
layout(binding=4) uniform sampler2D CartoonTex;

uniform int BlurEnabled = 1;
uniform int EdgeEnabled = 1;
uniform float AlphaFactor = 0.85f;

void main()
{
    ivec2 pix = ivec2(gl_FragCoord.xy);

    vec4 phong   = texelFetch(PhongTex,   pix, 0);
    vec4 blur    = texelFetch(BlurTex,    pix, 0);
    vec4 edge    = texelFetch(EdgeTex,    pix, 0);
    vec4 cartoon = texelFetch(CartoonTex, pix, 0);

    if (BlurEnabled == 0 && EdgeEnabled == 0)
    {
        FragColor = phong;
        return;
    }

    if (BlurEnabled == 1 && EdgeEnabled == 0)
    {
        FragColor = blur;
        return;
    }

    // Sobel output: white = edge, black = non-edge
    float edgeMask = clamp(edge.r, 0.0f, 1.0f);

    // black/white edge-filter view: black edges on white background
    vec3 edgeView = vec3(1.0f - edgeMask);

    // Ctrl+scroll blends cartoon -> black/white edge-view
    vec3 blended = mix(cartoon.rgb, edgeView, AlphaFactor);

    // But always keep actual detected edges black so they stay visible
    vec3 result = mix(blended, vec3(0.0f), edgeMask);

    FragColor = vec4(result, 1.0f);
}
)"