#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

// SSBO
layout (std430, binding = 0) buffer Matrices
{
    mat4 models[];
};

// Uniforms
uniform mat4 view;
uniform mat4 projection;
uniform uint nb_boids;

// out VS_OUT {
//     vec3 pos;
//     vec3 normal;
// } vs_out;
//

void main()
{
//     vs_out.pos = pos;
//     vs_out.normal = normal;
    gl_Position = projection * view * models[gl_InstanceID] * vec4(pos, 1.0);
}
