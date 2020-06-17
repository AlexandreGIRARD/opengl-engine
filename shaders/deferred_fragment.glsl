#version 450 core

struct material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

in vec3 frag_normal;
in vec3 frag_pos;

uniform material mtl;

layout (location = 0) out vec3 color;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 position;
layout (location = 3) out vec3 specular;

void main()
{
    color = mtl.diffuse;
    normal = vec4(frag_normal, mtl.shininess);
    position = vec4(frag_pos, 1.0);
    specular = mtl.specular;
}
