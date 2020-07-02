#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 clip_plane;

out vec3 frag_pos;
out vec3 frag_normal;

void main ()
{

    frag_pos = vec3(model * vec4(pos, 1.0));
    frag_normal = normalize(mat3(transpose(inverse(model))) * normal);
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
