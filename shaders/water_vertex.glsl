#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 frag_pos;
out vec3 frag_normal;
out vec2 frag_uv;

void main()
{
    frag_pos = model * vec4(pos, 1.0);
    frag_normal = normalize(mat3(transpose(inverse(model))) * normal);
    frag_uv = uv;
    gl_Position = projection * view * frag_pos;
}
