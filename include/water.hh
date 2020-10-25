#pragma once

#include "deferred.hh"
#include "program.hh"
#include "model.hh"
#include "camera.hh"
#include "point_light.hh"
#include "directional_light.hh"

using shared_model = std::shared_ptr<Model>;
using shared_light = std::shared_ptr<PointLight>;

class Water
{
public:
    Water(int width, int height, Model &water_surface, float y);
    void setup_program(DirectionalLight sun_light, std::vector<shared_light> lights);
    void setup_deferred(uint width, uint height);
    void render(std::vector<shared_model> models, Deferred &def, Skybox &skybox);
    void render_abv_surface(std::vector<shared_model> models, Deferred &def, Skybox &skybox);
    void render_sub_surface(Deferred &def);

private:
    Model _water_surface;
    Deferred _deferred_sky;
    program _water;
    program _sky;
    program _sub;
    program _fog;

    uint _reflection_FBO;
    uint _refraction_FBO;

    uint _reflection_tex;
    uint _depth_tex;
    uint _dudv;
    uint _normal_map;
    uint _fog_depth;
    uint _backbuffer_color;

    vec4 _clip_reflection;
    vec4 _clip_refraction;

    int _width;
    int _height;
    float _y;

    float _wave_speed = 0.001f;
    float _move_offset = 0.f;
};
