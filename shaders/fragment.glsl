#version 450 core

in vec3 light_vect;
in vec3 view_vect;
in vec4 shadow_uv;

out vec4 color;

uniform sampler2D def_color;
uniform sampler2D def_normal;
uniform sampler2D def_pos;
uniform sampler2D def_specular;
uniform sampler2D shadow_tex;

void main()
{
    // // Shadow coef computing
    float shadow = 1.0;
    if (texture(shadow_tex, shadow_uv.xy).x < shadow_uv.z - 0.005)
        shadow = 0.5;

    vec2 norm_coord = gl_FragCoord.xy / vec2(1920, 1080);
    vec3 _color = texture(def_color, norm_coord).xyz;
    vec3 _normal = texture(def_normal, norm_coord).xyz;
    vec3 _spec = texture(def_specular, norm_coord).xyz;
    float _shininess = texture(def_normal, norm_coord)[3];

    // Ambient
    vec3 ambient = 0.1 * _color;

    // Diffuse
    float diff_coef = max(dot(light_vect, _normal), 0.0);
    vec3 diffuse = _color * diff_coef;

    // Specular
    vec3 reflect_vect = normalize(reflect(-light_vect, _normal));
    float spec_coef = pow(max(dot(view_vect, reflect_vect), 0.0), _shininess * 128);
    vec3 spec = _spec * spec_coef;

    color = vec4((ambient + diffuse + spec) * shadow, 1.0);
}
