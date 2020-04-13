#version 450 core

#define SCALE -0.04

struct material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};


in vec3 frag_pos;
in vec3 frag_normal;
in vec3 light_vect;
in vec3 view_vect;
in vec4 shadow_uv;

out vec4 color;

uniform material mtl;
// uniform vec3 light_color;
uniform sampler2D shadow_map;

void main()
{
    // Shadow coef computing
    float shadow = 1.0;
    if (texture(shadow_map, shadow_uv.xy).z < shadow_uv.z)
        shadow = 0.5;

    // Ambient
    vec3 ambient = 0.1 * mtl.diffuse;

    // Diffuse
    float diff_coef = max(dot(light_vect, frag_normal), 0.0);
    vec3 diffuse = mtl.diffuse * diff_coef;

    // Specular
    vec3 reflect_vect = normalize(reflect(-light_vect, frag_normal));
    float spec_coef = pow(max(dot(view_vect, reflect_vect), 0.0), mtl.shininess);
    vec3 spec = mtl.specular * spec_coef;


    // color = vec4((ambient + diffuse) * shadow, 1.0);
    // color = vec4(1.0,1.0,1.0,1.0)  * shadow;
    color = texture(shadow_map, shadow_uv.xy);
    // fragColor = vec4(gl_FragCoord.z);
}
