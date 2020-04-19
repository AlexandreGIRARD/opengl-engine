#include <iostream>

#include <light.hh>

Light::Light(light_type type, vec3 pos, vec3 color, vec3 intensity)
{
    _pos = pos;
    _color = color;
    _intensity = intensity;
    _type = type;
    _id = _nb_lights;
    _nb_lights++;
    set_shadow_framebuffer();
}

uint Light::set_shadow_framebuffer()
{
    // Shadow texture for light
    glGenTextures(1, &_map);
    glBindTexture(GL_TEXTURE_2D, _map);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


    // FBO for the light
    glGenFramebuffers(1, &_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _map, 0);

    glDrawBuffer(GL_NONE);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Light number: "<< _id << " | Incomplete FBO" << std::endl;
        exit(-1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return _FBO;
}

void Light::setup_program(vec3 direction, vec3 optional_pos)
{
    _program = program();
    _program.add_shader("shadow_vertex.glsl", GL_VERTEX_SHADER);
    _program.add_shader("shadow_fragment.glsl", GL_FRAGMENT_SHADER);
    _program.link();
    _program.use();

    Camera shadow_cam;
    if (_type == DIRECTIONAL)
        shadow_cam = Camera(optional_pos, direction, vec3(0, 1, 0));
    else
        shadow_cam = Camera(_pos, direction, vec3(0, 1, 0));
    _view = shadow_cam.look_at();
    _program.addUniformMat4(_view, "view");

    mat4 projection = mat4(1.0);
    if (_type == DIRECTIONAL)
        _projection = ortho<float>(-10, 10, -10, 10, -10, 20);
    else
        _projection = perspective(radians(60.0f), 1.0f, 0.1f, 100.0f);
    _program.addUniformMat4(_projection, "projection");

}

void Light::draw_shadow_map(std::vector<Model> models)
{
    _program.use();
    glCullFace(GL_FRONT);
    glViewport(0,0,1024,1024);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glDrawBuffer(GL_NONE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto model : models)
        model.draw(_program);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::set_light_in_program(program p)
{
    p.use();

    p.addUniformVec3(_pos, "light" + _id + ".pos");
    p.addUniformMat4(_view, "light" + _id + ".view");
    p.addUniformMat4(_projection, "light" + _id + ".projection");
    // p.addUniformVec3(_color. "light" + _id + ".color");
}

uint Light::get_light_id()
{
    return _id;
}

uint Light::get_map()
{
    return _map;
}

mat4 Light::get_view()
{
    return _view;
}

mat4 Light::get_projection()
{
    return _projection
}
