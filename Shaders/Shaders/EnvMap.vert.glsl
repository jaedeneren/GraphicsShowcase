R"(
#version 330 core

layout(location=0) in vec3 VertexPosition;
layout(location=1) in vec3 VertexNormal;

uniform mat4 M;
uniform mat4 u_view;
uniform mat4 u_proj;
uniform int  u_isSkybox; 

out vec3 vWorldPos;
out vec3 vWorldNrm;
out vec3 vObjPos;

void main()
{
    vObjPos = VertexPosition;

    if (u_isSkybox == 1)
    {
        // remove translation so skybox stays centered
        mat4 V = mat4(mat3(u_view));

        vec4 pos = u_proj * V * vec4(VertexPosition, 1.0);

        // force depth to far plane
        gl_Position = pos.xyww;

        // dummy values (not used in skybox branch anyway)
        vWorldPos = vec3(0.0);
        vWorldNrm = vec3(0.0, 1.0, 0.0);
        return;
    }

    vec4 worldPos = M * vec4(VertexPosition, 1.0);
    vWorldPos = worldPos.xyz;

    mat3 N = mat3(transpose(inverse(M)));
    vWorldNrm = normalize(N * VertexNormal);

    gl_Position = u_proj * u_view * worldPos;
}
)"
