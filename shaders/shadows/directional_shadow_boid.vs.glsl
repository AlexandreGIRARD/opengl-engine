#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

// SSBO
layout (std430, binding = 0) buffer Matrices
{
    mat4 models[];
};

// Uniforms
uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 model = models[gl_InstanceID];
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
