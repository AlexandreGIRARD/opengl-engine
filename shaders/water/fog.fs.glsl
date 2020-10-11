#version 450 core

in VS_OUT {
    vec2 uv;
} fs_in;

uniform sampler2D depth_tex;
uniform sampler2D color_tex;

out vec4 color;

void main()
{
    vec4 fog_color = vec4(0, 0.2, 0.6, 0.1);
    vec4 tex_color = texture(color_tex, fs_in.uv);
    float depth = (texture(depth_tex, fs_in.uv).r * 10 - 9) / 2;
    color = mix(tex_color, fog_color, depth);
    color.a = 0.2;
}
