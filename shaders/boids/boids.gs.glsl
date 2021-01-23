#version 450 core

layout (triangles, invocations = 32) in;
layout (triangle_strip, max_vertices = 256) out; // Max for my gpu

in VS_OUT {
    vec3 pos;
    vec3 normal;
} gs_in[];

// out GS_OUT {
//     vec4 pos;
//     vec3 normal;
// } gs_out;

// SSBO
layout (std430, binding = 0) buffer Matrices
{
    mat4 models[];
};

// Uniforms
uniform mat4 view;
uniform mat4 projection;
uniform uint nb_boids;

// MACRO
#define MAX_INVOCATIONS  1;
#define MAX_VERTICES     256;

void draw_triangle(int i, uint boid_id)
{
    // gs_out.pos = models[boid_id] * vec4(gs_in[i].pos, 1.0);
    // gs_out.normal = transpose(inverse(mat3(models[boid_id]))) * gs_in[i].normal;
    gl_Position = projection * view * models[boid_id] * vec4(gs_in[i].pos, 1.0);
}

void main()
{
    uint n_draw = nb_boids / MAX_INVOCATIONS;

    for (uint i = 0; i < nb_boids; i++)
    {
        draw_triangle(0, i);
        EmitVertex();

        draw_triangle(1, i);
        EmitVertex();

        draw_triangle(2, i);
        EmitVertex();

        EndPrimitive();
    }
}
