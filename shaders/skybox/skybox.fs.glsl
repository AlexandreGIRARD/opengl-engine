#version 450 core

in VS_OUT {
    vec3 cube_coord;
} fs_in;

uniform samplerCube skybox;

out vec4 color;

void main()
{
    color = texture(skybox, fs_in.cube_coord);
}
