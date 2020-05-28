#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_normal;
out vec4 frag_pos;

void main()
{
    frag_pos = projection * view * model * vec4(pos, 1.0);
    frag_normal = normalize(mat3(model) * normal);
    gl_Position = frag_pos;
}
