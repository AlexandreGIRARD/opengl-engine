#pragma once

#include <glm/glm.hpp>

#include "program.hh"
#include "camera.hh"

using namespace glm;

class Skybox
{
public:
    Skybox()
    {}
    Skybox(std::string path);
    void set_skybox_cube();
    void render(Camera &cam);

private:
    uint _skybox;
    uint _VAO, _VBO;
    program _program;
};
