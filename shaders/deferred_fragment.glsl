#version 450 core

struct material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct textures
{
    sampler2D diffuse;
    sampler2D normal;
    sampler2D height;
};

in VS_OUT {
    vec4 pos;
    vec3 normal;
    vec2 uv;
    mat3 TBN;
} fs_in;

uniform uint id_model;
uniform uint is_textured;
uniform material mtl;
uniform textures tex;

layout (location = 0) out vec3 color;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 position;
layout (location = 3) out vec3 specular;

void normal_component()
{
    color = mtl.diffuse;
    normal = vec4(fs_in.normal, mtl.shininess);
    specular = mtl.specular;
}

void texture_component()
{
    color = texture(tex.diffuse, fs_in.uv).rgb;
    vec3 tmp_normal = texture(tex.normal, fs_in.uv).rgb * 2.0 - 1.0;
    tmp_normal = normalize(fs_in.TBN * tmp_normal);
    normal = vec4(tmp_normal, 0.2);
    specular = vec3(0.7);
}

void main()
{
    if (is_textured == 1)
        texture_component();
    else
        normal_component();
    position = fs_in.pos;

}
