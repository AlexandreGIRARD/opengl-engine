#version 450 core

layout (location = 0) in vec3 pos;

out VS_OUT {
    vec3 cube_coord;
} vs_out;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    vs_out.cube_coord = pos;
    vec4 tmp = projection * view * vec4(pos, 1.0);
    gl_Position = tmp.xyww;
}
