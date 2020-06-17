#include "water.hh"

Water::Water(int width, int height, Model &water_surface, float y)
    : _water_surface(water_surface),
      _width(width),
      _height(height)
{
    // Clip plane init
    _clip_reflection = vec4(0, 1, 0, -y);
    _clip_refraction = vec4(0, -1, 0, y);

    // Reflection texture
    glGenTextures(1, &_reflection_tex);
    glBindTexture(GL_TEXTURE_2D, _reflection_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Refraction texture
    glGenTextures(1, &_refraction_tex);
    glBindTexture(GL_TEXTURE_2D, _refraction_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    uint depth;
    glGenTextures(1, &depth);
    glBindTexture(GL_TEXTURE_2D, depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // TODO all in one fbo
    // Reflection FBO
    glGenFramebuffers(1, &_reflection_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _reflection_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _reflection_tex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_TEXTURE_2D, depth, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(-1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Refraction FBO
    glGenFramebuffers(1, &_refraction_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _refraction_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _refraction_tex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT, GL_TEXTURE_2D, depth, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        exit(-1);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Create program for rendering reflection and refraction texture
    _middle = program();
    _middle.add_shader("clipped_vertex.glsl", GL_VERTEX_SHADER);
    _middle.add_shader("clipped_fragment.glsl", GL_FRAGMENT_SHADER);
    _middle.link();

    // Create main water program
    _water = program();
    _water.add_shader("water_vertex.glsl", GL_VERTEX_SHADER);
    _water.add_shader("water_fragment.glsl", GL_FRAGMENT_SHADER);
    _water.link();
}

void Water::setup_program(DirectionalLight sun_light, std::vector<shared_light> lights)
{
    _middle.use();
    mat4 projection = mat4(1.0);
    projection = perspective(radians(60.0f), (float)_width / (float)_height, 0.1f, 100.0f);
    _middle.addUniformMat4(projection, "projection");
    sun_light.set_light_in_program(_middle);
    for (auto light : lights)
        light->set_light_in_program(_middle);
}

void Water::render(std::vector<shared_model> models, Camera cam)
{
    _middle.use();
    mat4 view = cam.look_at();
    _middle.addUniformMat4(view, "view");
    _middle.addUniformVec3(cam.get_position(), "cam_pos");

    //Enabling clip no avoid useless output
    glEnable(GL_CLIP_DISTANCE0);

    // Reflection texture to render
    _middle.addUniformVec4(_clip_reflection, "clip_plane");
    glBindFramebuffer(GL_FRAMEBUFFER, _reflection_FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    for (auto model : models)
        model->draw(_middle);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Refraction texture to render

    // Water Rendering
    glDisable(GL_CLIP_DISTANCE0);
}
