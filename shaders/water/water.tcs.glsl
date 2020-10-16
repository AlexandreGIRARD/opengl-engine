#version 450 core

in VS_OUT {
    vec4 pos;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
} tcs_in[];

out TCS_OUT {
    vec4 pos;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
} tcs_out[];

layout (vertices = 3) out;


void main()
{
    // Tessellation values for triangle subdivision
    gl_TessLevelOuter[0] = 6;
    gl_TessLevelOuter[1] = 6;
    gl_TessLevelOuter[2] = 6;

    gl_TessLevelInner[0] = 6;

    // Forwarding input value to output
    tcs_out[gl_InvocationID].pos = tcs_in[gl_InvocationID].pos;
    tcs_out[gl_InvocationID].uv = tcs_in[gl_InvocationID].uv;
    tcs_out[gl_InvocationID].normal = tcs_in[gl_InvocationID].normal;
    tcs_out[gl_InvocationID].tangent = tcs_in[gl_InvocationID].tangent;
}
