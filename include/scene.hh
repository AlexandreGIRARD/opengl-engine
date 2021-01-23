#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <json.hpp>

#include "camera.hh"
#include "model.hh"
#include "directional_light.hh"
#include "point_light.hh"
#include "deferred.hh"
#include "water.hh"
#include "normal_material.hh"
#include "textured_material.hh"
#include "skybox.hh"
#include "boids.hh"

using shared_camera = std::shared_ptr<Camera>;
using shared_lights = std::vector<std::shared_ptr<PointLight>>;
using shared_materials = std::vector<std::shared_ptr<Material>>;
using shared_models = std::vector<std::shared_ptr<Model>>;
using shared_swarms = std::vector<std::shared_ptr<Boids>>;


class Scene
{
public:
    Scene(const char *path);

    void parse_json(nlohmann::json &j);
    void add_light(nlohmann::json &j);
    void add_textured_material(nlohmann::json &j);
    void add_normal_material(nlohmann::json &j);
    void add_model(nlohmann::json &j);
    void add_swarm(nlohmann::json &j);

    void render(GLFWwindow *window, float delta, float xpos, float ypos);


private:
    uint _width, _height;
    shared_camera _camera;
    Deferred _deferred;
    Skybox _skybox;
    DirectionalLight _sun;
    shared_lights _lights;
    shared_materials _materials;
    shared_models _models;
    shared_swarms _swarms;

};
