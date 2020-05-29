#version 450 core

in vec3 frag_pos;

uniform vec3 light_pos;

layout (location = 0) out float dist;

void main()
{
    dist = length(frag_pos - light_pos);
}
