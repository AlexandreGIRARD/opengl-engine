#include "water.hh"

#include "util.hh"
#include <iostream>

Water::Water(int width, int height, Model &water_surface, float y)
    : _water_surface(water_surface),
      _deferred_sky(width, height, false),
      _width(width),
      _height(height),
      _y(y)
{
    // Clip plane init
    _clip_reflection = vec4(0, 1, 0, -y);
    _clip_refraction = vec4(0, -1, 0, y);

    // Reflection texture
    glGenTextures(1, &_reflection_tex);
    glBindTexture(GL_TEXTURE_2D, _reflection_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Refraction texture
    glGenTextures(1, &_depth_tex);
    glBindTexture(GL_TEXTURE_2D, _depth_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    uint depth;
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // TODO all in one fbo
    // Reflection FBO
    glGenFramebuffers(1, &_reflection_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _reflection_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _reflection_tex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(-1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Refraction FBO
    glGenFramebuffers(1, &_refraction_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _refraction_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth_tex, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(-1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create program for rendering reflection texture
    _sky = program();
    _sky.add_shader("water/reflection.vs.glsl", GL_VERTEX_SHADER);
    _sky.add_shader("water/reflection.fs.glsl", GL_FRAGMENT_SHADER);
    _sky.link();

    // Create above water program
    _water = program();
    _water.add_shader("water/water.vs.glsl", GL_VERTEX_SHADER);
    _water.add_shader("water/water.tcs.glsl", GL_TESS_CONTROL_SHADER);
    _water.add_shader("water/water.tes.glsl", GL_TESS_EVALUATION_SHADER);
    _water.add_shader("water/water.gs.glsl", GL_GEOMETRY_SHADER);
    _water.add_shader("water/water.fs.glsl", GL_FRAGMENT_SHADER);
    _water.link();

    // Create under water program
    _sub = program();
    _sub.add_shader("water/underwater.vs.glsl", GL_VERTEX_SHADER);
    _sub.add_shader("water/water.tcs.glsl", GL_TESS_CONTROL_SHADER);
    _sub.add_shader("water/water.tes.glsl", GL_TESS_EVALUATION_SHADER);
    _sub.add_shader("water/water.gs.glsl", GL_GEOMETRY_SHADER);
    _sub.add_shader("water/underwater.fs.glsl", GL_FRAGMENT_SHADER);
    _sub.link();

    // Create Water fog program
    _fog = program();
    _fog.add_shader("water/fog.vs.glsl", GL_VERTEX_SHADER);
    _fog.add_shader("water/fog.fs.glsl", GL_FRAGMENT_SHADER);
    _fog.link();

    // Fog depth texture
    glGenTextures(1, &_fog_depth);
    glBindTexture(GL_TEXTURE_2D, _fog_depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &_backbuffer_color);
    glBindTexture(GL_TEXTURE_2D, _backbuffer_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load textures
    _dudv = load_texture("textures/water/dudv.jpg");
    _normal_map = load_texture("textures/water/normal.jpg");
}

void Water::setup_program(DirectionalLight sun_light, std::vector<shared_light> lights)
{
    // Reflection program init uniform
    auto gprogram = _deferred_sky.get_program();
    mat4 projection = perspective(radians(60.0f), (float)_width / (float)_height, 0.01f, 50.0f);
    gprogram.addUniformMat4(projection, "projection");
    auto light_program = _deferred_sky.get_final();
    sun_light.set_light_in_program(light_program);
    for (auto light : lights)
        light->set_light_in_program(light_program);

    // Water program init uniform
    _water.use();
    _water.addUniformMat4(projection, "projection");
    sun_light.set_light_in_program(_water);
    for (auto light : lights)
        light->set_light_in_program(_water);

    // Under Water program init uniform
    _sub.use();
    _sub.addUniformMat4(projection, "projection");
    sun_light.set_light_in_program(_sub);
    for (auto light : lights)
        light->set_light_in_program(_sub);
}

void Water::render(std::vector<shared_model> models, Camera cam, Deferred &def, Skybox &skybox)
{
    _move_offset += _wave_speed;
    // _move_offset = _move_offset >= 1.f ? 0.f : _move_offset;
    if (cam.get_position().y >= -1) // TODO use water_surface.y and below surface
        render_abv_surface(models, cam, def, skybox);
    else
        render_sub_surface(cam, def);
}

void Water::render_sub_surface(Camera cam, Deferred &def)
{
    // Render sub water surface
    _sub.use();
    mat4 view = cam.look_at();
    vec3 pos = cam.get_position();
    _sub.addUniformMat4(view, "view");
    _sub.addUniformVec3(pos, "cam_pos");
    _sub.addUniformFloat(_move_offset, "move_offset");
    _sub.addUniformTexture(0, "dudv_map");
    _sub.addUniformFloat(glfwGetTime(), "t");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _dudv);
    _sub.addUniformTexture(1, "normal_map");
    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, _normal_map);

    //Bind refraction texture
    _sub.addUniformTexture(2, "refraction_tex");
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, def.get_output());

    _water_surface.draw_patches(_sub);


    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, _backbuffer_color);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, 1920, 1080, 0);
    glBindTexture(GL_TEXTURE_2D, _fog_depth);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, 1920, 1080, 0);

    // render water fog using depth buffer fromn backbuffer
    _fog.use();
    _fog.addUniformTexture(0, "depth_tex");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _fog_depth);
    _fog.addUniformTexture(1, "color_tex");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _backbuffer_color);
    def.render_screen_quad();

}

void Water::render_abv_surface(std::vector<shared_model> models, Camera cam, Deferred &def, Skybox &skybox)
{
    _water.use();
    mat4 view = cam.look_at();
    vec3 pos = cam.get_position();
    _water.addUniformMat4(view, "view");
    _water.addUniformVec3(pos, "cam_pos");
    _water.addUniformFloat(glfwGetTime(), "t");

    //Enabling clip no avoid useless output
    glEnable(GL_CLIP_DISTANCE0);

    // Deferred reflection texture
    float new_y = 2 * (cam.get_position().y - _y);
    cam.set_position_y(cam.get_position().y - new_y);
    cam.invert_pitch();
    view = cam.look_at();
    auto deferred_program = _deferred_sky.get_program();
    deferred_program.use();
    deferred_program.addUniformVec4(_clip_reflection, "clip_plane");
    _deferred_sky.update_viewport(view, pos);
    _deferred_sky.gbuffer_render(models);
    _deferred_sky.render();
    _deferred_sky.render_skybox(skybox, cam);

    // Water Rendering
    glDisable(GL_CLIP_DISTANCE0);
    _water.use();
    _water.addUniformFloat(_move_offset, "move_offset");
    _water.addUniformTexture(0, "dudv_map");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _dudv);
    _water.addUniformTexture(1, "normal_map");
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, _normal_map);

    // Bind reflection texture
    _water.addUniformTexture(2, "reflection_tex");
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, _deferred_sky.get_output());

    //Bind refraction texture
    _water.addUniformTexture(3, "refraction_tex");
    glActiveTexture(GL_TEXTURE0+3);
    glBindTexture(GL_TEXTURE_2D, def.get_output());
    // glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, _width, _height, 0);

    // Depth texture
    _water.addUniformTexture(4, "depth_tex");
    glActiveTexture(GL_TEXTURE0+4);
    glBindTexture(GL_TEXTURE_2D, def.get_depth());

    _water_surface.draw_patches(_water);
}
