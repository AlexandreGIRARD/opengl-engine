#pragma once

#include <memory>

#include "program.hh"
#include "model.hh"

class Deferred
{
public:
    Deferred(int width, int height);
    void update_viewport(mat4 &view, mat4 &projection);
    void render(std::vector<std::shared_ptr<Model>> models);
    void set_textures(program &p);
private:
    program _program;
    uint _FBO;
    uint _colors;
    uint _normals;
    uint _specular;
    uint _position; // Optional
};
