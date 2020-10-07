#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 clip_plane;

out VS_OUT {
    vec4 pos;
    vec3 normal;
    vec2 uv;
    mat3 TBN;
} vs_out;

void main()
{
    vs_out.pos = model * vec4(pos, 1.0);
    vs_out.normal = normalize(mat3(transpose(inverse(model))) * normal);
    vs_out.uv = uv;

    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    // vec3 B = normalize(vec3(model * vec4(bitangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    vs_out.TBN = transpose(mat3(T, B, N));

    gl_ClipDistance[0] = dot(model * vec4(pos, 1.0), clip_plane);
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
