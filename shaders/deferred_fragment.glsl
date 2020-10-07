#version 450 core

#define SCALE -0.01

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
uniform vec3 cam_pos;
uniform material mtl;
uniform textures tex;

layout (location = 0) out vec3 color;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 position;
layout (location = 3) out vec3 specular;

float relief_mapping(vec2 dp, vec2 ds)
{
    const int linear_search_steps = 20;
    const int binary_search_steps = 5;

    float size = 1.0 / linear_search_steps;
    float depth = 0.0;

    float best_depth = 1.0;

    for (int i = 0; i < linear_search_steps; i++)
    {
        depth += size;
        float off_depth = texture(tex.height, dp + ds*depth).x;
        if (depth >= off_depth)
        {
            best_depth = depth;
            break;
        }
    }
    depth = best_depth;
    for (int i = 0; i < binary_search_steps; i++)
    {
        size *= 0.5;
        float off_depth = texture(tex.height, dp + ds*depth).x;
        if (depth >= off_depth)
        {
            best_depth = depth;
            depth -= 2 * size;
        }
        depth += size;
    }

    return best_depth;
}

void normal_component()
{
    color = mtl.diffuse;
    normal = vec4(fs_in.normal, mtl.shininess);
    specular = mtl.specular;
}

void texture_component()
{
    // Relief Mapping
    vec3 tangent_view = fs_in.TBN * normalize(cam_pos - fs_in.pos.xyz);
    vec2 ds = tangent_view.xy / tangent_view.z * SCALE;
    float depth = relief_mapping(fs_in.uv, ds);
    vec2 uv = fs_in.uv + ds * depth;
    // uv = fs_in.uv;

    color = texture(tex.diffuse, uv).rgb;
    vec3 tmp_normal = texture(tex.normal, uv).rgb * 2.0 - 1.0;
    tmp_normal = normalize(fs_in.TBN * tmp_normal);
    normal = vec4(tmp_normal, 0.2);
    specular = vec3(0.4);
}

void main()
{
    if (is_textured == 1)
        texture_component();
    else
        normal_component();
    position = fs_in.pos;

}
