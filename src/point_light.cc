#include <point_light.hh>

#include <iostream>

PointLight::PointLight(vec3 pos, vec3 color, vec3 intensity)
    : Light(color, intensity)
    , _pos(pos)
{
    set_shadow_framebuffer();
    set_views();
}

uint PointLight::set_shadow_framebuffer()
{
    //
    glGenTextures(1, &_depth);
    glBindTexture(GL_TEXTURE_2D, _depth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

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
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depth, 0);

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
    _program.add_shader("point_shadow_vertex.glsl", GL_VERTEX_SHADER);
    _program.add_shader("point_shadow_fragment.glsl", GL_FRAGMENT_SHADER);
    _program.link();
    _program.use();

    _projection = perspective(radians(90.0f), 1.0f, 0.1f, 100.0f);;
    _program.addUniformMat4(_projection, "projection");

    _program.addUniformVec3(_pos, "light_pos");

}

void PointLight::draw_shadow_map(std::vector<std::shared_ptr<Model>> models)
{
    _program.use();
    glViewport(0,0,2048,2048);
    glClearColor(FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
    glCullFace(GL_FRONT);

    for (auto i=0; i < 6; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _faces[i], _map, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        _program.addUniformMat4(_views[i], "view");
        for (auto model : models)
            model->draw(_program);
    }
    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void PointLight::set_light_in_program(program p)
{
    p.use();

    auto tmp_id = std::to_string(_id);
    p.addUniformUint(1, ("light" + tmp_id + ".type").c_str());
    p.addUniformVec3(_pos, ("light" + tmp_id + ".pos").c_str());
    p.addUniformMat4(_view, ("light" + tmp_id + ".view").c_str());
    p.addUniformMat4(_projection, ("light" + tmp_id + ".projection").c_str());
    // p.addUniformVec3(_color. "light" + _id + ".color");
}
