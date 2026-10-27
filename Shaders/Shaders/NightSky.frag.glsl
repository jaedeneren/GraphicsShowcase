R"(
#version 330 core

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

struct MaterialInfo
{
    vec3  Ka;
    vec3  Kd;
    vec3  Ks;
    float Shininess;
    float effect;   // 0=phong, 1=discard, 2=cartoon, 3=checkerboard
};
uniform MaterialInfo material;

struct LightInfo
{
    vec4 Position;  // View-space
    vec3 La;
    vec3 Ld;
    vec3 Ls;
};
uniform LightInfo lights[8];
uniform int numLights;

// -------------------------------------------------------
// Fog
// -------------------------------------------------------
const float FOG_MIN   = 5.0;
const float FOG_MAX   = 40.0;
const vec3  FOG_COLOR = vec3(0.01, 0.01, 0.05);

vec3 applyFog(vec3 color, float depth)
{
    float t = clamp((depth - FOG_MIN) / (FOG_MAX - FOG_MIN), 0.0, 1.0);
    return mix(color, FOG_COLOR, t);
}

// -------------------------------------------------------
// Blinn-Phong
// -------------------------------------------------------
vec3 blinnPhong(vec3 pos, vec3 norm)
{
    vec3 ambient  = vec3(0.0);
    vec3 diffuse  = vec3(0.0);
    vec3 specular = vec3(0.0);

    int count = max(numLights, 1);
    for (int i = 0; i < count; ++i)
    {
        vec3 lightDir = (lights[i].Position.w == 0.0)
            ? normalize(vec3(lights[i].Position))
            : normalize(vec3(lights[i].Position) - pos);

        vec3 viewDir = normalize(-pos);
        vec3 halfway = normalize(lightDir + viewDir);

        ambient += lights[i].La * material.Ka;
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += lights[i].Ld * material.Kd * diff;
        float spec = (diff > 0.0) ? pow(max(dot(norm, halfway), 0.0), material.Shininess) : 0.0;
        specular += lights[i].Ls * material.Ks * spec;
    }

    return ambient + diffuse + specular;
}

// -------------------------------------------------------
// Cartoon
// -------------------------------------------------------
float quantise(float value, int levels)
{
    float scaleFactor = 1.0 / float(levels);
    return floor(value * float(levels)) * scaleFactor;
}

vec3 cartoon(vec3 pos, vec3 norm)
{
    vec3 lightDir = (lights[0].Position.w == 0.0)
        ? normalize(vec3(lights[0].Position))
        : normalize(vec3(lights[0].Position) - pos);

    vec3 viewDir = normalize(-pos);
    vec3 halfway = normalize(lightDir + viewDir);

    float diff = max(dot(norm, lightDir), 0.0);
    float spec = (diff > 0.0) ? pow(max(dot(norm, halfway), 0.0), material.Shininess) : 0.0;

    vec3 ambient  = lights[0].La * material.Ka;
    vec3 diffuse  = lights[0].Ld * material.Kd * quantise(diff, 2);
    vec3 specular = lights[0].Ls * material.Ks * quantise(spec, 2);

    if (abs(dot(viewDir, norm)) < 0.2)
        return vec3(0.0);

    return ambient + diffuse + specular;
}

// -------------------------------------------------------
// Discard - circular holes cut into the mesh using UV
// A 4x4 grid of circles punched through the surface
// -------------------------------------------------------
vec3 discardCircles(vec2 uv, vec3 pos, vec3 norm)
{
    vec2  tiled = fract(uv * 4.0);
    float dist  = length(tiled - vec2(0.5));
    if (dist < 0.35)
        discard;

    return blinnPhong(pos, norm);
}

// -------------------------------------------------------
// Checkerboard
// -------------------------------------------------------
vec3 checkerboard(vec2 uv, vec3 pos, vec3 norm)
{
    vec2  scaled  = floor(uv * 90.0);
    float checker = mod(scaled.x + scaled.y, 2.0);
    vec3  base    = (checker < 1.0) ? vec3(0.9) : vec3(0.1);

    vec3 lightDir = (lights[0].Position.w == 0.0)
        ? normalize(vec3(lights[0].Position))
        : normalize(vec3(lights[0].Position) - pos);

    float diff = max(dot(norm, lightDir), 0.0);
    return base * (0.2 + 0.8 * diff);
}

// -------------------------------------------------------
// Main
// -------------------------------------------------------
void main()
{
    vec3  n     = normalize(Normal);
    float depth = length(Position);

    vec3 color;

    if (material.effect == 1.0)         // Discard - circular holes (cube)
    {
        color = discardCircles(TexCoord, Position, n);
    }
    else if (material.effect == 2.0)    // Cartoon (teapot)
    {
        color = cartoon(Position, n);
    }
    else if (material.effect == 3.0)    // Checkerboard (plane)
    {
        color = checkerboard(TexCoord, Position, n);
    }
    else                                // Phong (sphere)
    {
        color = blinnPhong(Position, n);
    }

    color = applyFog(color, depth);
    FragColor = vec4(color, 1.0);
}
)"
