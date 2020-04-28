#pragma once

#include <light.hh>

class DirectionalLight : public Light
{
public:
    DirectionalLight(vec3 vect, vec3 color, vec3 intensity);
    uint set_shadow_framebuffer() override;
    void draw_shadow_map(std::vector<std::shared_ptr<Model>> models) override;
    void set_light_in_program(program p) override;
    void setup_program(vec3 direction, vec3 optional_pos);
private:
    vec3 _vect;
};
