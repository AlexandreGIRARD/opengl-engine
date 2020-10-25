#pragma once

#include "deferred.hh"
#include "program.hh"

class Deferred;

class AmbientOcclusion
{
public:
    AmbientOcclusion()
    {}
    AmbientOcclusion(uint width, uint height);
    uint render(Deferred &def);
    uint get_map();

private:

    void generate_kernels();
    void generate_noise();

    program _main;
    program _blur;

    uint _noise_map;
    uint _occlusion_map;
    uint _blured_map;


    uint _main_fbo;
    uint _blur_fbo;

    uint _width;
    uint _height;

    std::vector<vec3> _kernels;
};
