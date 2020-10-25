#include "scene.hh"

#include <fstream>
#include <iostream>

Scene::Scene(const char *path)
{
    std::ifstream file(path);
    nlohmann::json j;
    file >> j;

    parse_json(j);

    // Setup sun & lights
    _sun.setup_program(vec3(0,0,0), vec3(0, 0.5, -1));// <-- Update with camera position
    for (auto light : _lights)
        light->setup_program();

    // Setup Deferred rendering
    _deferred = Deferred(_width, _height, _camera, true);
    _sun.set_light_in_program(_deferred.get_final());
    for (auto light : _lights)
        light->set_light_in_program(_deferred.get_final());
    // Setup occlusion ambient
}

void Scene::parse_json(nlohmann::json &j)
{
    // Setup main variables
    _height = j["height"];
    _width = j["width"];

    // Setup main Camera
    auto j_cam = j["camera"];
    _camera = std::make_shared<Camera>(vec3(j_cam["position"][0], j_cam["position"][1], j_cam["position"][2]),
            vec3(j_cam["forward"][0] , j_cam["forward"][1] , j_cam["forward"][2]),
            vec3(j_cam["up"][0]      , j_cam["up"][1]      , j_cam["up"][2]),
            j_cam["fov"], j_cam["speed"], j_cam["near"], j_cam["far"], (float)_width/_height);

    // Setup skybox
    _skybox = Skybox(j["skybox"]);


    // Setup sun
    auto j_sun = j["sun"];
    _sun = DirectionalLight(vec3(j_sun["vect"][0] , j_sun["vect"][1] , j_sun["vect"][2]),
            vec3(j_sun["color"][0], j_sun["color"][1], j_sun["color"][2]),
            j_sun["intensity"]);

    // Setup lights
    _lights = shared_lights();
    for (auto j_light : j["lights"])
        add_light(j_light);


    // Setup materials
    _materials = shared_materials();
    for (auto j_mtl : j["materials"])
    {
        if (j_mtl["type"] == "normal")
            add_normal_material(j_mtl);
        else if (j_mtl["type"] == "textured")
            add_textured_material(j_mtl);
    }

    // Setup models
    _models = shared_models();
    for (auto j_model : j["models"])
        add_model(j_model);

    // Setup water
    // model_trans = translate(model, vec3(0, -1, 0));
    // model_scale = scale(model_trans, vec3(5, 5, 5));
    // auto water_surface = Model("models/wave.obj", model_scale, mat2);
    // Water water = Water(width, height, water_surface, -1);
    // water.setup_program(sun, lights);
}

void Scene::add_light(nlohmann::json &j)
{
    _lights.emplace_back(std::make_shared<PointLight>(
                vec3(j["pos"][0]  , j["pos"][1]  , j["pos"][2]),
                vec3(j["color"][0], j["color"][1], j["color"][2]),
                j["intensity"]
                ));

}

void Scene::add_textured_material(nlohmann::json &j)
{
    _materials.emplace_back(std::make_shared<Textured_Material>(j["path"]));

}

void Scene::add_normal_material(nlohmann::json &j)
{
    _materials.emplace_back(std::make_shared<Normal_Material>(
                vec3(j["diffuse"][0] , j["diffuse"][1] , j["diffuse"][2]),
                vec3(j["specular"][0], j["specular"][1], j["specular"][2]),
                j["shininess"]
                ));

}

void Scene::add_model(nlohmann::json &j)
{
    _models.emplace_back(std::make_shared<Model>( j["path"], _materials[j["material_id"]],
                vec3(j["translation"][0], j["translation"][1], j["translation"][2]),
                vec3(j["rotation"][0]   , j["rotation"][1]   , j["rotation"][2]),
                vec3(j["scale"][0]      , j["scale"][1]      , j["scale"][2]),
                j["degree"]
                ));

}


void Scene::render(GLFWwindow *window, float delta, float xpos, float ypos)
{
    _camera->update(window, delta, xpos, ypos);

    // Update sun position from camera pos
    // sun.update_position(cam.get_position()); <-- TODO

    /*
       HANDLE ANIMATION FOR MODELS HERE
       */

    // First pass deferred rendering --> fill G_BUFFER
    _deferred.update_viewport();
    _deferred.gbuffer_render(_models);

    // Shadow computing
    _sun.draw_shadow_map(_models);
    for (auto light : _lights)
        light->draw_shadow_map(_models);
    glViewport(0, 0, _width, _height);

    // Second pass deferred rendering --> render using G_BUFFER & SHADOWS
    _deferred.set_shadow_maps(_sun, _lights);
    _deferred.render();
    _deferred.render_skybox(_skybox);
    _deferred.bind_fbo_to_backbuffer(); // TODO Need to be erased not fucked up tweak

    // Water rendering
    // water.render(models, cam, deferred, skybox);



}
