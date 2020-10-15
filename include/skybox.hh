#pragma once

#include <glm/glm.hpp>

#include "program.hh"

using namespace glm;

class Skybox
{
public:
    Skybox(std::string path, mat4 &projection);
    void set_skybox_cube();
    void display(mat4 &view);

private:
    uint _skybox;
    uint _VAO, _VBO;
    program _program;
};
