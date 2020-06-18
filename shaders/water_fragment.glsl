#version 450 core

in vec4 frag_pos;
in vec3 frag_normal;
in vec2 frag_uv;

uniform float move_offset;
uniform vec3 cam_pos;

uniform sampler2D dudv_map;
uniform sampler2D normal_map;

uniform sampler2D reflection_tex;
uniform sampler2D refraction_tex;
uniform sampler2D depth_tex;

out vec4 color;

float get_water_depth(vec2 coord)
{
    float near = 0.01;
    float far  = 50.0;
    float depth = texture(depth_tex, coord).a;
    float floor_distance = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
    float water_distance = 2.0 * near * far / (far + near - (2.0 * gl_FragCoord.z - 1.0) * (far - near));
    float water_depth = floor_distance - water_distance;
    return water_depth;
}

void main()
{
    vec2 refract_coord = gl_FragCoord.xy / vec2(1920, 1080);
    vec2 reflect_coord = vec2(refract_coord.x, 1 - refract_coord.y);

    float depth = get_water_depth(refract_coord);

    // Distortion for texture coordinates
    vec2 distorded_coord = frag_uv + vec2(move_offset, 0);
    distorded_coord = texture(dudv_map, distorded_coord).rg;
    vec2 dudv = (distorded_coord * 2.0 - 1.0) * 0.1 * depth;
    // dudv *= clamp()
    vec3 normal = texture(normal_map, distorded_coord).xyz * 2.0 - 1.0;

    refract_coord += dudv;
    refract_coord = clamp(refract_coord, 0.001, 0.999);
    reflect_coord += dudv;
    reflect_coord = clamp(reflect_coord, 0.001, 0.999);

    vec4 reflect = texture(reflection_tex, reflect_coord);
    vec4 refract = texture(refraction_tex, refract_coord);

    // Fresnel
    vec3 view_vect = normalize(cam_pos - frag_pos.xyz);
    float fresnel = pow(dot(view_vect, frag_normal), 1.8);

    color = mix(reflect, refract, fresnel);
    color = mix(color, vec4 (0., 0.1, 0.4, 0.8), depth);
    color.a = clamp(depth / 5.0, 0, 1);
}
