#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out; // 6 faces * 3 vertices

in vec3 frag_pos[];

in VS_OUT {
    vec3 pos;
    mat4 model;
} gs_in[];

uniform mat4 projection;
uniform mat4 views[6];

out GS_OUT {
    vec3 pos;
} gs_out;


void main()
{
    for (int i = 0; i < 6; i++)
    {
        gl_Layer = i;
        for (int j = 0; j < 3; j++)
        {
            gs_out.pos = vec3(gs_in[j].model * vec4(gs_in[j].pos, 1.0));
            gl_Position = projection * views[i] * gs_in[j].model * vec4(gs_in[j].pos, 1.0);
            EmitVertex();
        }
        EndPrimitive();
    }
}
