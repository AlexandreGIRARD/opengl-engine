#version 450 core

in GS_OUT {
    vec3 pos;
} fs_in;

uniform vec3 light_pos;

layout (location = 0) out float dist;

void main()
{
    dist = length(fs_in.pos - light_pos);
}
