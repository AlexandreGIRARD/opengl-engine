#pragma once

#include "light.hh"

class PointLight : Light
{
public:
    PointLight(vec3 pos, vec3 color, vec3 intensity);
    uint set_shadow_framebuffer() override;
    void draw_shadow_map(std::vector<std::shared_ptr<Model>> models) override;
    void set_light_in_program(program p) override;
    void setup_program(vec3 direction);
    void set_views();

private:
    vec3 _pos;
    std::vector<mat4> _views;
};
