#version 450 core

#define NB_PTS_LIGHTS 2

struct material
{
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct point_light
{
    vec3 pos;
    vec3 color;
    float intensity;
};

struct sun_light
{
    vec3 vect;
    vec3 color;
    float intensity;
};

in vec3 frag_pos;
in vec3 frag_normal;

uniform vec3 cam_pos;
uniform point_light lights[NB_PTS_LIGHTS];
uniform sun_light sun;
uniform material mtl;

out vec4 color;

vec3 get_color(vec3 light_vect, vec3 light_color, float light_intensity, vec3 view_vect)
{
    // Ambient
    vec3 ambient = 0.1 * mtl.diffuse;
    // Diffuse
    float diff_coef = max(dot(light_vect, frag_normal), 0.0);
    vec3 diffuse = mtl.diffuse * diff_coef;
    // Specular
    vec3 half_vect = normalize(light_vect + view_vect);
    float spec_coef = pow(max(dot(frag_normal, half_vect), 0), mtl.shininess * 128);
    vec3 spec = mtl.specular * spec_coef;

    return (ambient + diffuse + spec) * light_color * light_intensity;
}

void main()
{
    vec3 view_vect = normalize(cam_pos - frag_pos);

    vec3 final_color = get_color(sun.vect, sun.color, sun.intensity, view_vect);
    for (int i = 0; i < NB_PTS_LIGHTS; i++) {
        vec3 light_vect = normalize(lights[i].pos - frag_pos);
        final_color += get_color(light_vect, lights[i].color, lights[i].intensity, view_vect);
    }
    color = vec4(final_color, gl_FragCoord.z);
}
