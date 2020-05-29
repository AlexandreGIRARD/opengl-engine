#version 450 core

layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 sun_view;
uniform mat4 sun_projection;

const mat4 bias_matrix = mat4(vec4(0.5, 0.0, 0.0, 0.0),
                              vec4(0.0, 0.5, 0.0, 0.0),
                              vec4(0.0, 0.0, 0.5, 0.0),
                              vec4(0.5, 0.5, 0.5, 1.0));

out vec4 shadow_uv;

void main()
{
    gl_Position = projection * view * model * vec4(pos, 1.0);

    mat4 light_matrix = sun_projection * sun_view * model;
    shadow_uv = bias_matrix*light_matrix * vec4(pos, 1.0);
}
