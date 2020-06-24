#pragma once

#include "program.hh"

class Material
{
public:
    Material()
    {}
    virtual void set_uniforms(program p) = 0;
};
