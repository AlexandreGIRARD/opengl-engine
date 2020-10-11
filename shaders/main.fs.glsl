#version 450 core

#define NB_PTS_LIGHTS 1
#define BIAS 0.001

struct point_light
{
    vec3 pos;
    vec3 color;
    float intensity;
    samplerCube map;
};

struct sun_light
{
    vec3 vect;
    vec3 color;
    float intensity;
    sampler2D map;
};

struct deferred_info
{
    vec3 color;
    vec3 normal;
    vec3 pos;
    vec3 spec;
    float shininess;
    uint id;
};

in vec2 frag_uv;

uniform uint shadow;
uniform vec3 cam_pos;
uniform point_light lights[NB_PTS_LIGHTS];
uniform sun_light sun;

uniform sampler2D def_color;
uniform sampler2D def_normal;
uniform sampler2D def_position;
uniform sampler2D def_specular;

uniform mat4 sun_view;
uniform mat4 sun_projection;

const mat4 bias_matrix = mat4(vec4(0.5, 0.0, 0.0, 0.0),
                              vec4(0.0, 0.5, 0.0, 0.0),
                              vec4(0.0, 0.0, 0.5, 0.0),
                              vec4(0.5, 0.5, 0.5, 1.0));

out vec4 color;

/*
 * Compute shadow coef for the given light
*/
float point_shadow_coef(point_light light, vec3 pos, vec3 light_vect)
{
    float d_receiver = length(pos - light.pos);
    float d_blocker = texture(light.map, -light_vect).r;
    // In light
    if (d_receiver < d_blocker + BIAS)
        return 1.0;
    // In shadow
    float penumbra = (d_receiver - d_blocker) * 0.5 / d_blocker;
    // return 1.0 - (0.5 *abs(penumbra));
    return 0.5;
}

/*
 * Compute shadow coef for the sunlight with uv_coord and shadow_map with pcf3
*/
float sun_shadow_coef(vec4 shadow_uv, sampler2D shadow_map, int filter_size)
{
    float total = 0;
    vec2 size = 1.0 / textureSize(shadow_map, 0);
    for (float y = -filter_size; y <= filter_size; y+=1.5) {
       for (float x = -filter_size; x <= filter_size; x+=1.5) {
           float tmp = texture(shadow_map, shadow_uv.xy + vec2(x, y) * size).r;
           total += shadow_uv.z - BIAS > tmp ? 1.0 : 0.0;
       }
    }
    total /= (filter_size * 2.0 + 1.0) * (filter_size * 2.0 + 1.0);

    return max(1.0 - (total * shadow_uv.w), 0.5);
}

/*
 * Compute lighting for the sun light or scene global light
*/
vec3 get_sun_light(sun_light light, deferred_info infos, vec3 view_vect, vec4 shadow_uv)
{

    float shadow = sun_shadow_coef(shadow_uv, light.map, 3);
    // shadow = 1.0;
    vec3 ambient = 0.1 * infos.color;

    // Diffuse
    float diff_coef = max(dot(light.vect, infos.normal), 0.0);
    vec3 diffuse = infos.color * diff_coef;

    // Specular
    vec3 half_vect = normalize(light.vect + view_vect);
    float spec_coef = pow(max(dot(infos.normal, half_vect), 0), infos.shininess * 128);
    vec3 spec = infos.spec * spec_coef;

    return (ambient + diffuse + spec) * shadow * light.color * light.intensity;
}

/*
 * Compute lighting for a given point light
*/
vec3 get_light(point_light light, deferred_info infos, vec3 view_vect)
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
    float spec_coef = pow(max(dot(infos.normal, half_vect), 0), infos.shininess * 128);
    vec3 spec = infos.spec * spec_coef;

    return (ambient + diffuse + spec) * shadow * light.color * light.intensity;
}

void main()
{

    // Get deferred informations
    vec2 norm_coord = frag_uv;
    deferred_info infos;
    infos.color = texture(def_color, norm_coord).xyz;
    infos.normal = texture(def_normal, norm_coord).xyz;
    infos.pos = texture(def_position, norm_coord).xyz;
    infos.spec = texture(def_specular, norm_coord).xyz;
    infos.shininess = texture(def_normal, norm_coord)[3];
    infos.id = uint(texture(def_position, norm_coord).w);

    // Shadow uv computation for sun shadow

    mat4 light_matrix = sun_projection * sun_view;
    vec4 shadow_uv = bias_matrix*light_matrix * vec4(infos.pos, 1.0);

    vec3 view_vect = normalize(cam_pos - infos.pos);

    vec3 final_color = get_sun_light(sun, infos, view_vect, shadow_uv);
    for (int i = 0; i < NB_PTS_LIGHTS; i++)
        final_color += get_light(lights[i], infos, view_vect);


    color = vec4(final_color, gl_FragCoord.z);
}
