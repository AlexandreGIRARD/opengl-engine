#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out VS_OUT {
    vec3 pos;
} vs_out;

void main()
{
    vs_out.pos = pos;
    gl_Position = vec4(pos, 1.0);
}
