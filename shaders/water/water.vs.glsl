#version 450 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
    vec4 pos;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
} vs_out;

// out VS_OUT {
//     vec4 pos;
//     vec2 uv;
//     mat3 TBN;
// } vs_out;

void main()
{
    vs_out.pos = vec4(pos, 1.0);
    vs_out.uv = uv;
    vs_out.normal = normal;
    vs_out.tangent = tangent;
    gl_Position = vs_out.pos;

    // vs_out.pos = model * vec4(pos, 1.0);
    // vs_out.uv = uv;
    // vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    // vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    // T = normalize(T - dot(T, N) * N);
    // vec3 B = cross(N, T);
    // vs_out.TBN = transpose(mat3(T, B, N));
    // gl_Position = projection * view * vs_out.pos;
}
