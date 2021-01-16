#version 450 core

struct boid_t
{
    vec4 pos;
    vec4 dir;
};

layout (std430, binding = 0) buffer Positions
{
    vec4 pos[];
};

layout (std430, binding = 1) buffer Directions
{
    vec4 dir[];
};

layout (std430, binding = 2) buffer Matrices
{
    mat4 mat[];
};

uniform uint max_boids;

layout (local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;
void main()
{
    uint gid = gl_GlobalInvocationID.x;
    if (gid < max_boids)
    {
        // Update here
        boid_t b;
        b.pos = pos[gid];
        b.dir = dir[gid];
        b.pos += vec4(0.001, 0.001, 0.001, 0);
        pos[gid] = b.pos;
        dir[gid] = b.dir;
        mat[gid] = mat4(gid);
    }
}
