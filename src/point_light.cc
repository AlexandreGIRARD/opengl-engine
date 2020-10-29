#include <point_light.hh>

#include <iostream>

PointLight::PointLight(vec3 pos, vec3 color, float intensity)
    : Light(color, intensity)
    , _pos(pos)
{
    set_shadow_framebuffer();
    set_views();
}

uint PointLight::set_shadow_framebuffer()
{
    glGenTextures(1, &_depth);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _depth);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Iterate over 6 faces
    for (auto i=0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    // Shadow cube map
    glGenTextures(1, &_map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _map);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Iterate over 6 faces
    for (auto i=0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, 2048, 2048, 0, GL_RED, GL_FLOAT, NULL);


    // FBO for the light
    glGenFramebuffers(1, &_FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _map, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _depth, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Light number: "<< _id << " | Incomplete FBO" << std::endl;
        exit(-1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return _FBO;
}

void PointLight::set_views()
{
    _views.emplace_back(lookAt(_pos, _pos + vec3(1 , 0, 0), vec3(0, -1, 0)));
    _views.emplace_back(lookAt(_pos, _pos + vec3(-1, 0, 0), vec3(0, -1, 0)));
    _views.emplace_back(lookAt(_pos, _pos + vec3(0 , 1, 0), vec3(0, 0, -1)));
    _views.emplace_back(lookAt(_pos, _pos + vec3(0 ,-1, 0), vec3(0, 0, -1)));
    _views.emplace_back(lookAt(_pos, _pos + vec3(0 , 0, 1), vec3(0, -1, 0)));
    _views.emplace_back(lookAt(_pos, _pos + vec3(0 , 0,-1), vec3(0, -1, 0)));
}

void PointLight::setup_program()
{
    _program = program();
    _program.add_shader("shadows/point_shadow.vs.glsl", GL_VERTEX_SHADER);
    _program.add_shader("shadows/point_shadow.gs.glsl", GL_GEOMETRY_SHADER);
    _program.add_shader("shadows/point_shadow.fs.glsl", GL_FRAGMENT_SHADER);
    _program.link();
    _program.use();

    _projection = perspective(radians(90.0f), 1.0f, 0.1f, 100.0f);;
    _program.addUniformMat4(_projection, "projection");

    _program.addUniformVec3(_pos, "light_pos");

}

void PointLight::draw_shadow_map(std::vector<std::shared_ptr<Model>> models)
{
    glViewport(0,0,2048,2048);
    _program.use();
    for (auto i=0; i < 6; i++)
    _program.addUniformMat4(_views[i], ("views[" + std::to_string(i) + "]").c_str());

    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glCullFace(GL_FRONT);
    glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    for (auto model : models)
        model->draw(_program);

    glCullFace(GL_BACK);
    glClearColor(0, 0, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointLight::draw_shadow_map(std::vector<std::shared_ptr<Model>> models, Boids &swarm)
{
    glViewport(0,0,2048,2048);
    _program.use();
    for (auto i=0; i < 6; i++)
    _program.addUniformMat4(_views[i], ("views[" + std::to_string(i) + "]").c_str());

    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glCullFace(GL_FRONT);
    glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    for (auto model : models)
        model->draw(_program);
    swarm.draw(_program);

    glCullFace(GL_BACK);
    glClearColor(0, 0, 0, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PointLight::set_light_in_program(program p)
{
    p.use();

    auto tmp_id = std::to_string(_id - 1);
    p.addUniformVec3(_pos, ("lights[" + tmp_id + "].pos").c_str());
    p.addUniformVec3(_color, ("lights[" + tmp_id + "].color").c_str());
    p.addUniformFloat(_intensity, ("lights[" + tmp_id + "].intensity").c_str());
}

void PointLight::set_shadow_cube(program p)
{
    auto tmp_id = std::to_string(_id - 1);
    p.addUniformTexture(5 + _id, ("lights[" + tmp_id + "].map").c_str());
    glActiveTexture(GL_TEXTURE5 + _id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _map);
}
