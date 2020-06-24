#pragma once

#include "material.hh"

#include <glm/glm.hpp>
using namespace glm;

class Normal_Material : public Material
{
public:
    Normal_Material(vec3 diffuse, vec3 specular, float shininess);
    void set_uniforms(program p) override;
private:
    vec3 _diffuse;
    vec3 _specular;
    float _shininess;
};
