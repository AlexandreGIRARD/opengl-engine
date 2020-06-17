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

out vec4 color;

void main()
{
    vec2 norm_coord = gl_FragCoord.xy / vec2(1920, 1080);
    vec2 reflect_coord = vec2(norm_coord.x, 1 - norm_coord.y);

    vec2 dudv = ((texture(dudv_map, frag_uv + vec2(move_offset, 0)).xy * 2.0) - 1.0) * 0.02;
    vec3 normal = texture(normal_map, frag_uv).xyz * 2.0 - 1.0;

    norm_coord += dudv;
    vec2 refract_coord = clamp(norm_coord, 0.001, 0.999);
    reflect_coord += dudv;
    reflect_coord = clamp(reflect_coord, 0.001, 0.999);

    vec4 reflec = texture(reflection_tex, reflect_coord);
    vec4 refrac = texture(refraction_tex, refract_coord);

    vec3 view_vect = normalize(cam_pos - frag_pos.xyz);
    float fresnel = pow(dot(view_vect, frag_normal), 1);

    color = mix(reflec, refrac, fresnel);
}
