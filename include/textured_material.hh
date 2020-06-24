#pragma once

#include "material.hh"
#include "util.hh"

class Textured_Material : public Material
{
public:
    Textured_Material(std::string path);
    void set_uniforms(program p) override;
private:
    uint _diffuse;
    uint _specular;
    uint _normal;
    uint _height;
};
