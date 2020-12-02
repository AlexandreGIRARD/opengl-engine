#include <directional_light.hh>

#include <iostream>

DirectionalLight::DirectionalLight(vec3 vect, vec3 color, float intensity)
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

void DirectionalLight::setup_program(vec3 direction, vec3 cam_pos)
{
    _program = program();
    _program.add_shader("shadows/directional_shadow.vs.glsl", GL_VERTEX_SHADER);
    _program.add_shader("shadows/directional_shadow.fs.glsl", GL_FRAGMENT_SHADER);
    _program.link();
    _program.use();

    _shadow_cam = Camera(cam_pos + vec3(0, 0.5, -1), direction, vec3(0, 1, 0));
    _view = _shadow_cam.look_at();
    _program.addUniformMat4(_view, "view");

    _projection = ortho<float>(-10, 10, -10, 10, -10, 20);
    _program.addUniformMat4(_projection, "projection");

}

void DirectionalLight::update_position(vec3 cam_pos)
{
    _program.use();
    _shadow_cam.set_position(cam_pos + vec3(0, 0.5, 1));
    _view = _shadow_cam.look_at();
    _program.addUniformMat4(_view, "view");
}

void DirectionalLight::draw_shadow_map(shared_models models)
{
    _program.use();
    glCullFace(GL_FRONT);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,2048,2048);
    glDrawBuffer(GL_NONE);

    for (auto model : models)
        model->draw(_program);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
}

void DirectionalLight::set_light_in_program(program p)
{
    p.use();

    auto tmp_id = std::to_string(_id);
    p.addUniformVec3(_vect, "sun.vect");
    p.addUniformVec3(_color, "sun.color");
    p.addUniformFloat(_intensity, "sun.intensity");
    p.addUniformMat4(_view, "sun_view");
    p.addUniformMat4(_projection,"sun_projection");
}

void DirectionalLight::set_shadow_map(program p)
{
    auto tmp_id = std::to_string(_id);
    p.addUniformTexture(5 , "sun.map");
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _map);
}
