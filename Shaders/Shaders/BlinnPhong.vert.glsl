R"(
#version 460 core

// Attributes (Per-vertex data)
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

// Uniforms (Global data)
uniform mat4 M;      // Model Matrix
uniform mat4 u_view; // View Matrix
uniform mat4 u_proj; // Projection Matrix

// Outputs to Fragment Shader
out vec3 v_pos;    // Position in View Space
out vec3 v_normal; // Normal in View Space

void main()
{
    // 1. Calculate View-Model Matrix (converts from Object -> View Space)
    mat4 VM = u_view * M;

    // 2. Transform Position to View Space
    // We use vec4(pos, 1.0) for points
    v_pos = vec3(VM * vec4(aPos, 1.0));

    // 3. Transform Normal to View Space
    // Normals need the "Normal Matrix" (transpose inverse of top-left 3x3 of VM)
    // We use mat3 to strip translation data.
    v_normal = mat3(transpose(inverse(VM))) * aNormal;

    // 4. Final Clip Space Position for the GPU to draw
    gl_Position = u_proj * VM * vec4(aPos, 1.0);
}
)"