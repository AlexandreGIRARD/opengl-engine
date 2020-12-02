#include "ambient_occlusion.hh"

#include <random>
#include <iostream>

#define NOISE_SIZE  4
#define KERNEL_SIZE 64
#define LERP(v0, v1, t) v0 + t * (v1 - v0)

AmbientOcclusion::AmbientOcclusion(uint width, uint height)
{
    _width  = width;
    _height = height;
    // Occlusion map
    glGenTextures(1, &_occlusion_map);
    glBindTexture(GL_TEXTURE_2D, _occlusion_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &_main_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _main_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _occlusion_map, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blured Occlusion
    glGenTextures(1, &_blured_map);
    glBindTexture(GL_TEXTURE_2D, _blured_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glGenFramebuffers(1, &_blur_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _blur_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _blur_fbo, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _main = program();
    _main.add_shader("occlusion/main.vs.glsl", GL_VERTEX_SHADER);
    _main.add_shader("occlusion/main.fs.glsl", GL_FRAGMENT_SHADER);
    _main.link();

    // _blur = program();
    // _blur.add_shader("occlusion/blur.vs.glsl", GL_VERTEX_SHADER);
    // _blur.add_shader("occlusion/blur.fs.glsl", GL_FRAGMENT_SHADER);
    // _blur.link();

    generate_kernels();
    generate_noise();
}

void AmbientOcclusion::generate_kernels()
{
    _kernels = std::vector<vec3>();

    // Init random generator
    std::uniform_real_distribution<float> dis_xy(-1.f, 1.f);
    std::uniform_real_distribution<float> dis_z(0.f, 1.f);
    std::default_random_engine gen;

    for (int i = 0; i < KERNEL_SIZE; i++)
    {
        vec3 tmp = vec3(dis_xy(gen), dis_xy(gen), dis_z(gen));
        tmp = normalize(tmp);
        tmp *= dis_z(gen);
        float scale = (float)i / KERNEL_SIZE;
        scale = LERP(0.1f, 1.0f, scale*scale); // Linear interpolation
        tmp *= scale;
        _kernels.emplace_back(tmp);
    }
}

void AmbientOcclusion::generate_noise()
{
    vec3 *noise = new vec3[NOISE_SIZE*NOISE_SIZE];
    std::uniform_real_distribution<float> dis(-1.f, 1.f);
    std::default_random_engine gen;

    // Initialize noise array
    for (uint i = 0; i < NOISE_SIZE*NOISE_SIZE; i++)
        noise[i] = normalize(vec3(dis(gen), dis(gen), 0.f));

    // Copy noise array to gpu noise texture
    glGenTextures(1, &_noise_map);
    glBindTexture(GL_TEXTURE_2D, _noise_map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, NOISE_SIZE, NOISE_SIZE, 0, GL_RGB, GL_FLOAT, noise);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    delete[] noise;
}

uint AmbientOcclusion::render(Deferred &def)
{
    _main.use();
    // Render oclusion map
    glBindFramebuffer(GL_FRAMEBUFFER, _main_fbo);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    mat4 projection = def.get_camera().get_projection();
    mat4 view = def.get_camera().look_at();
    _main.addUniformMat4(projection, "projection");
    _main.addUniformMat4(view, "view");
    // Set gbuffer output for computing occlusion
    _main.addUniformTexture(0, "def.position");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, def.get_pos());
    _main.addUniformTexture(1, "def.normal");
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, def.get_normal());
    // Set noise and kernel samples
    for (int i = 0; i < KERNEL_SIZE; i++)
        _main.addUniformVec3(_kernels[i], ("samples[" + std::to_string(i) + "]").c_str());
    _main.addUniformTexture(2, "noise");
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _noise_map);
    vec2 noise_scale(_width/float(NOISE_SIZE), _height/float(NOISE_SIZE));
    _main.addUniformVec2(noise_scale, "noise_scale");

    // render on screen space using deferred function
    def.render_screen_quad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return _occlusion_map;
}

uint AmbientOcclusion::get_map()
{
    return _occlusion_map;
    // return _blured_map;
}
