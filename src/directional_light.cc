#include <directional_light.hh>

#include <iostream>

DirectionalLight::DirectionalLight(vec3 vect, vec3 color, vec3 intensity)
    : Light(color, intensity)
    , _vect(vect)
{
    set_shadow_framebuffer();
}

uint DirectionalLight::set_shadow_framebuffer()
{
    // Shadow texture for light
    glGenTextures(1, &_map);
    glBindTexture(GL_TEXTURE_2D, _map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glBindTexture(GL_TEXTURE_2D, 0);


    GLuint rboId;
    glGenRenderbuffers(1, &rboId);
    glBindRenderbuffer(GL_RENDERBUFFER, rboId);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);

    // FBO for the light
    glGenFramebuffers(1, &_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _map, 0);
    // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Light number: "<< _id << " | Incomplete FBO" << std::endl;
        exit(-1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return _FBO;
}

void DirectionalLight::setup_program(vec3 direction, vec3 space_pos)
{
    _program = program();
    _program.add_shader("shadow_vertex.glsl", GL_VERTEX_SHADER);
    _program.add_shader("shadow_fragment.glsl", GL_FRAGMENT_SHADER);
    _program.link();
    _program.use();

    Camera shadow_cam = Camera(space_pos, direction, vec3(0, 1, 0));
    _view = shadow_cam.look_at();
    _program.addUniformMat4(_view, "view");

    _projection = ortho<float>(-10, 10, -10, 10, -10, 20);
    _program.addUniformMat4(_projection, "projection");

}

void DirectionalLight::draw_shadow_map(std::vector<std::shared_ptr<Model>> models)
{
    _program.use();
    glCullFace(GL_FRONT);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,2048,2048);
    // glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glDrawBuffer(GL_NONE);

    for (auto model : models)
        model->draw(_program);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void DirectionalLight::set_light_in_program(program p)
{
    p.use();

    auto tmp_id = std::to_string(_id);
    p.addUniformUint(0, ("light" + tmp_id + ".type").c_str());
    p.addUniformVec3(_vect, ("light" + tmp_id + ".pos").c_str());
    p.addUniformMat4(_view, ("light" + tmp_id + ".view").c_str());
    p.addUniformMat4(_projection, ("light" + tmp_id + ".projection").c_str());
    // p.addUniformVec3(_color. "light" + _id + ".color");
}
