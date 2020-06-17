#pragma once

#include <glad/glad.h>
#include <iostream>
#include <string>
#include <vector>

#include "program.hh"


class Skybox
{
    public:
        Skybox(std::vector<std::string> faces);
        unsigned int loadCubemap();
        void setup_program();
        void draw_cubemap(unsigned int cubemapTexture);

    private:
        std::vector<std::string> _faces;
        program _program;
};
