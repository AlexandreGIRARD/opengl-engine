#pragma once

#include <memory>

#include "program.hh"
#include "directional_light.hh"
#include "point_light.hh"
#include "model.hh"
#include "skybox.hh"
#include "ambient_occlusion.hh"

class AmbientOcclusion;

using shared_camera = std::shared_ptr<Camera>;
using shared_occlusion = std::shared_ptr<AmbientOcclusion>;


class Deferred
{
public:
    Deferred()
    {}
    Deferred(int width, int height, shared_camera cam, bool with_shadow);
    ~Deferred();
    void update_viewport();
    void gbuffer_render(std::vector<std::shared_ptr<Model>> models);

    void render();
    void render_screen_quad();
    void render_skybox(Skybox &skybox);

    void bind_fbo_to_backbuffer();

    uint get_pos();
    uint get_normal();
    uint get_depth();
    uint get_output();
    Camera get_camera();
    program get_final();
    program get_program();

    void set_textures(program &p);
    void set_camera(shared_camera camera);

    void set_screen_quad();
    void set_shadow_maps(DirectionalLight &sun, std::vector<std::shared_ptr<PointLight>> lights);

private:
    program _program;
    program _final;

    shared_occlusion _occlusion;
    shared_camera _camera;

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
