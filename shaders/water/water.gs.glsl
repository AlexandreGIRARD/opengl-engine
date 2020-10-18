#version 450 core

layout (triangles) in;

// TO CHANGE:
#define AMPLITUDE 0.2
#define X_SPEED 1.2
#define Z_SPEED 0.6
#define X_SCALE 0.8
#define Z_SCALE 0.4

in TES_OUT {
  vec4 pos;
  vec2 uv;
  vec3 normal;
  mat3 TBN;
} gs_in[];

uniform float t;

out GS_OUT {
  vec4 pos;
  vec2 uv;
  vec3 normal;
  mat3 TBN;
} gs_out;

layout (triangle_strip, max_vertices = 3) out;

vec3 wave_modification(vec3 pos)
{
    pos.y += AMPLITUDE * sin(X_SPEED * t + X_SCALE * pos.y);
    pos.y += AMPLITUDE * sin(Z_SPEED * t + Z_SCALE * pos.y);
    return pos;
}

vec4 wave_modification(vec4 pos)
{
    pos.y += AMPLITUDE * sin(X_SPEED * t + X_SCALE * pos.y);
    pos.y += AMPLITUDE * sin(Z_SPEED * t + Z_SCALE * pos.y);
    return pos;
}

void set_vertex(int i)
{
    gs_out.pos = wave_modification(gs_in[i].pos);
    gs_out.uv = gs_in[i].uv;
    gs_out.normal = gs_in[i].normal;
    gs_out.TBN = gs_in[i].TBN;
    gl_Position = wave_modification(gl_in[i].gl_Position);
}

void main()
{
    set_vertex(0);
    EmitVertex();

    set_vertex(1);
    EmitVertex();

    set_vertex(2);
    EmitVertex();

    EndPrimitive();
}
