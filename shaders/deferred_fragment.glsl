#version 450 core

struct material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 frag_normal;
in vec4 frag_pos;

layout (location = 0) out vec3 color;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec3 specular;

uniform material mtl;

void main()
{
    color = mtl.diffuse;
    normal = vec4(frag_normal, mtl.shininess);
    specular = mtl.specular;
}
