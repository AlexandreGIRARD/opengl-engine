#pragma once

#include <light.hh>

class DirectionalLight : public Light
{
public:
    DirectionalLight()
    {}
    DirectionalLight(vec3 vect, vec3 color, float intensity);
    uint set_shadow_framebuffer() override;
    void draw_shadow_map(shared_models models) override;
    void draw_shadow_map(shared_models models, shared_swarms swarms) override;
    void update_position(vec3 cam_pos);
    void set_light_in_program(program p) override;
    void set_shadow_map(program p);
    void setup_program(vec3 direction, vec3 optional_pos);
private:
    Camera _shadow_cam;
    vec3 _vect;
};
