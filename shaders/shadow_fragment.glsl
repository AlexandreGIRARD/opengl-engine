#version 450 core

layout (location = 0) out float dist;

void main()
{
    dist = gl_FragCoord.z;
}
