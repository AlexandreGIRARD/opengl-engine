#version 450 core

#define NB_PTS_LIGHTS 2

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

in VS_OUT {
  vec4 pos;
  vec3 normal;
  vec2 uv;
  mat3 TBN;
} fs_in;

uniform float move_offset;
uniform vec3 cam_pos;

uniform sampler2D dudv_map;
uniform sampler2D normal_map;

uniform sampler2D refraction_tex;

uniform point_light lights[NB_PTS_LIGHTS];
uniform sun_light sun;

out vec4 color;

vec4 get_sun_specular(sun_light light, vec3 view_vect, vec3 normal)
{
    vec3 half_vect = normalize(light.vect + view_vect);
    float spec_coef = pow(max(dot(normal, half_vect), 0), 16);
    return vec4(0.4) * spec_coef * light.intensity;
}

vec4 get_point_specular(point_light light, vec3 view_vect, vec3 normal)
{
    vec3 light_vect = normalize(light.pos - vec3(fs_in.pos));
    vec3 half_vect = normalize(light_vect + view_vect);
    float spec_coef = pow(max(dot(normal, half_vect), 0), 8);
    return vec4(0.4) * spec_coef;
}

void main()
{
    vec2 refract_coord = gl_FragCoord.xy / vec2(1920, 1080);

    // Distortion for texture coordinates
    vec2 distorded_coord = texture(dudv_map, fs_in.uv + vec2(move_offset, 0)).rg;
    distorded_coord = fs_in.uv + vec2(distorded_coord.x, distorded_coord.y + move_offset*0.5);
    vec2 dudv = (texture(dudv_map, distorded_coord).rg * 2.0 - 1.0) * 0.005;
    vec3 normal = texture(normal_map, distorded_coord).rgb * 2.0 - 1.0;
    normal = normalize(fs_in.TBN * normal);

    // Move coordinates
    refract_coord += dudv;
    refract_coord = clamp(refract_coord, 0.001, 0.999);

    vec4 refract = texture(refraction_tex, refract_coord);

    // Fresnel
    vec3 view_vect = normalize(cam_pos - fs_in.pos.xyz);
    float fresnel = pow(dot(view_vect, fs_in.normal), 1.6);

    vec4 specular = get_sun_specular(sun, view_vect, normal);
    for (int i = 0; i < NB_PTS_LIGHTS; i++)
        specular += get_point_specular(lights[i], view_vect, normal);

    // refract = mix(refract, vec4(0, 0.2, 0.6, 0.4), 0.4);
    color = refract + specular * 0.5;
    // color.a = clamp(depth / 5.0, 0, 1);
    // color = vec4(depth);
    // color = vec4(1,0,0,0);
}
