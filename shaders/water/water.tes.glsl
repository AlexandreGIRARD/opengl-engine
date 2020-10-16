#version 450 core

layout (triangles, equal_spacing) in;

in TCS_OUT {
    vec4 pos;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
} tes_in[];

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out TES_OUT {
    vec4 pos;
    vec2 uv;
    vec3 normal;
    mat3 TBN;
} tes_out;


vec2 interpolate(vec2 v0, vec2 v1, vec2 v2)
{
    return v0 * gl_TessCoord.x + v1 * gl_TessCoord.y + v2 * gl_TessCoord.z;
}

vec3 interpolate(vec3 v0, vec3 v1, vec3 v2)
{
    return v0 * gl_TessCoord.x + v1 * gl_TessCoord.y + v2 * gl_TessCoord.z;
}

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2)
{
    return v0 * gl_TessCoord.x + v1 * gl_TessCoord.y + v2 * gl_TessCoord.z;
}

void main()
{
    // Interpolate value from tessellation primitive generation
    vec4 position = interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position);
    vec4 pos = interpolate(tes_in[0].pos, tes_in[1].pos, tes_in[2].pos);
    vec2 uv = interpolate(tes_in[0].uv, tes_in[1].uv, tes_in[2].uv);
    vec3 normal = interpolate(tes_in[0].normal, tes_in[1].normal, tes_in[2].normal);
    vec3 tangent = interpolate(tes_in[0].tangent, tes_in[1].tangent, tes_in[2].tangent);

    // Compute to clip space
    tes_out.pos = model * pos;
    tes_out.uv = uv;
    tes_out.normal = normalize(mat3(transpose(inverse(model))) * normal);
    gl_Position = projection * view * tes_out.pos;

    // TBN Matrix computation
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    tes_out.TBN = transpose(mat3(T, B, N));
}
