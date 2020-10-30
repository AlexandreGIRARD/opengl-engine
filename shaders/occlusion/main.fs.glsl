#version 450 core

#define SAMPLES_SIZE 64
#define RADIUS 0.25 // To tweak parametrer
#define BIAS 0.0

struct deferred
{
    sampler2D position;
    sampler2D normal;
};

in vec2 frag_uv;

uniform deferred def;
uniform sampler2D noise;

uniform mat4 view;
uniform mat4 projection;
uniform vec3[SAMPLES_SIZE] samples;
uniform vec2 noise_scale;


layout (location = 0) out float occlusion;

void main()
{
    // Pos and normal is in model space
    vec3 pos = texture(def.position, frag_uv).xyz;
    vec3 normal = texture(def.normal, frag_uv).xyz;

    // Transfer pos to view space
    pos = vec3(view * vec4(pos, 1.0));
    normal = normalize(vec3(view * vec4(normal, 1.0)));

    vec3 rvec = normalize(texture(noise, frag_uv * noise_scale).xyz);

    vec3 tangent = normalize(rvec - normal * dot(rvec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    occlusion = 0.f;
    for (int i = 0; i < SAMPLES_SIZE; i++)
    {
        // get sample position in view space
        vec3 sample_pos = TBN * samples[i];
        sample_pos = sample_pos * RADIUS + pos;

        // translate sample to screen space
        vec4 offset = vec4(sample_pos, 1.f);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5f + 0.5f;

        // Get sample depth
        float sample_depth = texture(def.position, offset.xy).z;

        // Comparaison
        float range = smoothstep(0.0, 1.0, RADIUS / abs(pos.z - sample_depth));
        occlusion += (sample_depth >= sample_pos.z + BIAS ? 1.0 : 0.0) * range;
    }
    occlusion = 1.f - (occlusion / SAMPLES_SIZE);
    occlusion = 1;
}
