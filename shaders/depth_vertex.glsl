#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 clip_plane;

void main ()
{
    gl_ClipDistance[0] = dot(model * vec4(pos, 1.0), clip_plane);
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
