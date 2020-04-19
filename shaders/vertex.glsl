#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

uniform vec3 cam_pos;
uniform vec3 light_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 light_view;
uniform mat4 light_projection;

const mat4 bias_matrix = mat4(vec4(0.5, 0.0, 0.0, 0.0),
                              vec4(0.0, 0.5, 0.0, 0.0),
                              vec4(0.0, 0.0, 0.5, 0.0),
                              vec4(0.5, 0.5, 0.5, 1.0));

out vec3 frag_pos;
out vec3 frag_normal;
out vec3 view_vect;
out vec3 light_vect;
out vec4 shadow_uv;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);
    frag_pos = vec3(model * vec4(pos, 1.0));
    frag_normal = normalize(mat3(model) * normal);
    light_vect = normalize(light_pos - frag_pos);
    view_vect = normalize(cam_pos - frag_pos);

    mat4 light_matrix = light_projection * light_view * model;
    shadow_uv = bias_matrix*light_matrix * vec4(pos, 1.0);
}
