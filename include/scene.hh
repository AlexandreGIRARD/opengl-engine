#pragma once

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

class Scene
{
public:
    Scene(const char *path);

    void parse_json(nlohmann::json &j);
    void add_light(nlohmann::json &j);
    void add_textured_material(nlohmann::json &j);
    void add_normal_material(nlohmann::json &j);
    void add_model(nlohmann::json &j);

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
    Boids _swarm;
};

/*
Scene::Scene(char *json_file, bool kdtree)
{
    std::ifstream file(json_file);
    nlohmann::json j;
    file >> j;
    // Global info
    _height = j["height"];
    _width = j["width"];
    _camera = Camera(Vector4(j["camera"]["pos"]), Vector4(j["camera"]["up"]), Vector4(j["camera"]["forward"]), j["camera"]["fov"]);
    _fov = tan((_camera.get_fov() * (M_PI / 180)) * 0.5);
    if (_width > _height)
        _ratio_x = double(_width) / _height;
    else
        _ratio_y = double(_height) / _width;

    // Material
    for (auto mtl : j["materials"])
        _materials[mtl["name"]] = Material(mtl);

    // Objects
    if (!kdtree) {
        for (auto obj : j["objects"]) {
            if (obj["type"] == "sphere")
                add_object(std::make_shared<Sphere>(obj, _materials.find(obj["material"])->second));
            else if (obj["type"] == "plane")
                add_object(std::make_shared<Plane>(obj, _materials.find(obj["material"])->second));
            else if (obj["type"] == "object") {
                Parser p = Parser(obj, _materials.find(obj["material"])->second);
                auto triangles = p.parse();
                _objects.insert(_objects.begin(), triangles.begin(), triangles.end());
            }
        }
        _kdtree = nullptr;
        _scene_voxel = nullptr;
    } else {
        auto planes = std::vector<std::shared_ptr<Object>>();
        auto finite = std::vector<std::shared_ptr<Object>>();
        for (auto obj : j["objects"]) {
            if (obj["type"] == "sphere")
                finite.emplace_back(std::make_shared<Sphere>(obj, _materials.find(obj["material"])->second));
            else if (obj["type"] == "plane")
                add_object(std::make_shared<Plane>(obj, _materials.find(obj["material"])->second));
            else if (obj["type"] == "object") {
                Parser p = Parser(obj, _materials.find(obj["material"])->second);
                auto triangles = p.parse();
                finite.insert(finite.begin(), triangles.begin(), triangles.end());
            }
        }
        _scene_voxel = get_global_voxel(finite);
        _kdtree = construct_tree(finite, *_scene_voxel);
    }
    // Lights
    for (auto light : j["lights"])
    {
        if (light["type"] == "directional")
            add_light(std::make_shared<DirectionalLight>(light));
        else if (light["type"] == "point")
            add_light(std::make_shared<PointLight>(light));
    }
}*/
