#version 450 core

in vec3 light_vect;
in vec3 view_vect;
in vec4 shadow_uv;

out vec4 color;

uniform sampler2D def_color;
uniform sampler2D def_normal;
uniform sampler2D def_specular;
uniform sampler2D shadow_tex;


void main()
{
    // Shadow coef computing
    float shadow = 16.0;
    vec2 size = 1.0 / textureSize(shadow_tex, 0);
    for (float y = -1.5; y <= 1.5; y += 1.0) {
       for (float x = -1.5; x <= 1.5; x += 1.0) {
           float tmp = texture(shadow_tex, shadow_uv.xy + vec2(x, y) * size).r;
           shadow -= shadow_uv.z - 0.005 > tmp ? 1.0 : 0.0;
       }
    }
    shadow = shadow / 16.0 > 0.5 ? shadow / 16.0 : 0.5;

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
    vec3 half_vect = normalize(light_vect + view_vect);
    float spec_coef = pow(max(dot(_normal, half_vect), 0), _shininess * 128);
    vec3 spec = _spec * spec_coef;

    color = vec4((ambient + diffuse + spec) * shadow, 1.0);
}
