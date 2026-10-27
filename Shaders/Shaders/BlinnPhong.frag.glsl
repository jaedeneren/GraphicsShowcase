R"(
#version 460 core

out vec4 FragColor;

// Inputs from Vertex Shader
in vec3 v_pos;    // Position in View Space
in vec3 v_normal; // Normal in View Space

// Light Structure (Matches C++ Light struct)
struct Light {
    vec3 position; // Light Position (World Space)
    vec3 La;       // Ambient Intensity
    vec3 Ld;       // Diffuse Intensity
    vec3 Ls;       // Specular Intensity
};

// Material Structure (Matches C++ Material struct)
struct Material {
    vec3 Ka;          // Ambient Reflectivity
    vec3 Kd;          // Diffuse Reflectivity
    vec3 Ks;          // Specular Reflectivity
    float shininess;  // Shininess factor (higher = smaller, sharper highlight)
};

uniform Light light[1];    // We support 1 light for now
uniform Material material; // The material of the object being drawn
uniform mat4 u_view;       // We need View Matrix to transform Light Pos to View Space

void main()
{
    // --- 1. Setup Vectors (All in View Space) ---
    
    // Normal (N): Must be normalized (length 1)
    vec3 N = normalize(v_normal);
    
    // View Vector (V): Direction from fragment to camera (origin in view space)
    // Since we are in View Space, the camera is at (0,0,0). 
    // Vector TO camera is (0,0,0) - v_pos = -v_pos.
    vec3 V = normalize(-v_pos);

    // Light Position needs to be in View Space too!
    vec3 lightPosView = vec3(u_view * vec4(light[0].position, 1.0));
    
    // Light Vector (L): Direction from fragment to light
    vec3 L = normalize(lightPosView - v_pos);

    // Halfway Vector (H): For Blinn-Phong Specular
    vec3 H = normalize(L + V);

    // --- 2. Calculate Components ---

    // Ambient: Base color that is always there
    // If material doesn't define Ka, use Kd (Diffuse) as fallback
    vec3 ambient = light[0].La * material.Kd; 
    if (length(material.Ka) > 0.0) {
        ambient = light[0].La * material.Ka;
    }

    // Diffuse: "Dot Product" determines how much the surface faces the light
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = light[0].Ld * material.Kd * diff;

    // Specular: Blinn-Phong uses Dot(Normal, Halfway)
    float spec = 0.0;
    if (diff > 0.0) { // Only calculate specular if lit
        float specAngle = max(dot(N, H), 0.0);
        // Default shininess to 32 if 0
        float shininess = material.shininess;
        if (shininess <= 0.0) shininess = 32.0; 
        
        spec = pow(specAngle, shininess);
    }
    vec3 specular = light[0].Ls * material.Ks * spec;

    // --- 3. Combine ---
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
)"