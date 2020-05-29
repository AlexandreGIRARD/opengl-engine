#version 450 core

#define NB_PTS_LIGHTS 1
#define BIAS 0.001

struct point_light
{
    vec3 pos;
    vec3 color;
    vec3 intensity;
    samplerCube map;
};

struct info
{
    vec3 color;
    vec3 normal;
    vec3 pos;
    vec3 spec;
    float shininess;
};

in vec4 shadow_uv;

uniform vec3 cam_pos;
uniform point_light lights[NB_PTS_LIGHTS];

uniform sampler2D def_color;
uniform sampler2D def_normal;
uniform sampler2D def_position;
uniform sampler2D def_specular;
// uniform sampler2D shadow_map;

out vec4 color;

/*
 * Compute shadow coef for the given light
*/
float point_shadow_coef(point_light light, vec3 pos, vec3 light_vect)
{
    float dist = length(pos - light.pos);
    if (dist > texture(light.map, -light_vect).r + BIAS)
        return 0.5;
    return 1.0;
}

/*
 * Compute shadow coef for the sunlight with uv_coord and shadow_map
*/
float sun_shadow_coef(vec2 uv_coord, sampler2D shadow_map)
{
    float shadow = 16.0;
    vec2 size = 1.0 / textureSize(shadow_map, 0);
    for (float y = -1.5; y <= 1.5; y += 1.0) {
       for (float x = -1.5; x <= 1.5; x += 1.0) {
           float tmp = texture(shadow_map, uv_coord.xy + vec2(x, y) * size).r;
           shadow -= shadow_uv.z - BIAS > tmp ? 1.0 : 0.0;
       }
    }
    return shadow / 16.0 > 0.5 ? shadow / 16.0 : 0.5;
}

vec3 get_light(point_light light, info infos, vec3 view_vect)
{
    vec3 light_vect = normalize(light.pos - infos.pos);
    float shadow = point_shadow_coef(light, infos.pos,  light_vect);
    // Ambient
    vec3 ambient = 0.1 * infos.color;

    // Diffuse
    float diff_coef = max(dot(light_vect, infos.normal), 0.0);
    vec3 diffuse = infos.color * diff_coef;

    // Specular
    vec3 half_vect = normalize(light_vect + view_vect);
    float spec_coef = pow(max(dot(infos.normal, half_vect), 0), infos.shininess * 64);
    vec3 spec = infos.spec * spec_coef;

    return (ambient + diffuse + spec) * shadow;
}

void main()
{
    // Get deferred informations
    vec2 norm_coord = gl_FragCoord.xy / vec2(1920, 1080);
    info infos;
    infos.color = texture(def_color, norm_coord).xyz;
    infos.normal = texture(def_normal, norm_coord).xyz;
    infos.pos = texture(def_position, norm_coord).xyz;
    infos.spec = texture(def_specular, norm_coord).xyz;
    infos.shininess = texture(def_normal, norm_coord)[3];

    vec3 view_vect = normalize(cam_pos - infos.pos);

    vec3 final_color = vec3(0);
    for (int i = 0; i < NB_PTS_LIGHTS; i++)
        final_color += get_light(lights[i], infos, view_vect);


    color = vec4(final_color, 1.0);
}
