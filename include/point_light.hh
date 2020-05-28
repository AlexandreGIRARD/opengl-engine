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
    const uint _faces[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};
};
