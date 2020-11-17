#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

#include <iostream>

uint load_texture(const char *name);
uint load_cubemap(const std::string path, const std::string faces[6]);
float frame_rate(float time);
void GLAPIENTRY debug_callback(GLenum source,
                               GLenum type,
                               GLuint id,
                               GLenum severity,
                               GLsizei length,
                               const GLchar* message,
                               const void* userParam);

// Window callback
void quit_window(GLFWwindow *window);
void move_window(GLFWwindow *window, double *xpos, double *ypos);
float pause_rotation(GLFWwindow *window, float rad_off);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
GLFWwindow *init_window(uint width, uint height);
