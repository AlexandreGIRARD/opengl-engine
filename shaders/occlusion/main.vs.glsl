#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

out vec2 frag_uv;

void main()
{
    frag_uv = uv;
    gl_Position = vec4(pos, 1.0);
}
