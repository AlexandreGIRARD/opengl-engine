#version 450 core

struct material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform material mtl;

layout (location = 0) out vec3 color;

void main()
{
    color = mtl.diffuse;
}
