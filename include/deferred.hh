#pragma once

#include <memory>

#include "program.hh"
#include "directional_light.hh"
#include "point_light.hh"
#include "model.hh"
#include "skybox.hh"

class Deferred
{
public:
    Deferred(int width, int height, bool with_shadow);
    void update_viewport(mat4 &view, vec3 &position);
    void gbuffer_render(std::vector<std::shared_ptr<Model>> models);

    void render();
    void render_screen_quad();
    void render_skybox(Skybox &skybox, mat4 &view);

    void bind_fbo_to_backbuffer();

    uint get_pos();
    uint get_depth();
    uint get_output();
    program get_final();
    program get_program();

    void set_textures(program &p);
    void set_screen_quad();
    void set_shadow_maps(DirectionalLight &sun, std::vector<std::shared_ptr<PointLight>> lights);

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
    uint _shadow;
    uint _depth;
    uint _output;
};
