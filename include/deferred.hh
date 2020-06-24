#pragma once

#include <memory>

#include "program.hh"
#include "directional_light.hh"
#include "point_light.hh"
#include "model.hh"

class Deferred
{
public:
    Deferred(int width, int height);
    void update_viewport(mat4 &view, mat4 &projection, vec3 &position);
    void gbuffer_render(std::vector<std::shared_ptr<Model>> models);
    void render();
    void set_textures(program &p);
    uint get_depth();
    program get_program();
    uint get_output();
    void set_shadow_maps(DirectionalLight &sun, std::vector<std::shared_ptr<PointLight>> lights);
    void set_screen_quad();
    void render_screen_quad();

private:
    program _program;
    program _final;

    uint _quad_VAO;
    uint _quad_VBO;

    uint _FBO;
    uint _final_FBO;

    uint _colors;
    uint _normals;
    uint _specular;
    uint _position;
    uint _depth;
    uint _output;
};
