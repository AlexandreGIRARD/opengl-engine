#version 450 core

uniform sampler2D reflection_tex;
uniform sampler2D refraction_tex;

out vec4 color;

void main()
{
    vec2 norm_coord = gl_FragCoord.xy / vec2(1920, 1080);
    vec2 invert_coord = vec2(norm_coord.x, 1 - norm_coord.y);
    vec4 reflec = texture(reflection_tex, invert_coord);
    vec4 refrac = texture(refraction_tex, norm_coord);
    color = mix(reflec, refrac, 0.5);
}
