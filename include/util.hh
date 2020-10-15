#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>

#include <iostream>

uint load_texture(const char *name);
uint load_cubemap(const std::string path, const std::string faces[6]);
