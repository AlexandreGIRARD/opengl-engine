#include "boids.hh"

#include <random>
#include <cmath>
#include <glm/gtx/vector_angle.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

int Boids::_nb_swarms = 0;

Boids::Boids(std::string path, std::shared_ptr<Material> mat, int size, float bound, float speed, float fov, float separation, float alignment, float cohesion)
    : _model(path, mat)
    , _size(size)
    , _bound(bound)
    , _fov(fov)
    , _speed(speed)
    , _angle(radians(fov))
    , _separation(separation)
    , _alignment(alignment)
    , _cohesion(cohesion)
{
    _id = _nb_swarms++;
    // Init random generator
    std::uniform_real_distribution<float> dis_bound(-bound, bound);
    std::uniform_real_distribution<float> dis(-1.f, 1.f);
    std::random_device gen;

    // Initialize random boids
    auto positions  = std::vector<vec4>(size);
    auto directions = std::vector<vec4>(size);
    for (int i = 0; i < _size; i++)
    {
        positions[i]  = vec4(dis_bound(gen), dis_bound(gen), dis_bound(gen), 1);
        directions[i] = normalize(vec4(dis(gen), dis(gen), dis(gen), 1));
    }

    _program = program();
    _program.add_shader("boids/boids.cs.glsl", GL_COMPUTE_SHADER);
    _program.link();

    //TODO: Init SSBO boids
    _program.use();
    glGenBuffers(1, &_pos_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _pos_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _size * sizeof(vec4), positions.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _pos_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &_dir_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _dir_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _size * sizeof(vec4), directions.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _dir_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &_mat_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _mat_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _size * sizeof(mat4), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _mat_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    _program.addUniformUint(_size, "max_boids");
}

void Boids::update(vec3 &follow, vec3 &predator) // GPU Version
{
    _program.use();
    update_ui();
    update_uniforms(follow, predator);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _pos_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _dir_ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _mat_ssbo);


    glDispatchCompute(ceil(_size / 1024.f), 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Boids::update_ui()
{
    const auto title = ("Swarm " + std::to_string(_id)).c_str();
    ImGui::Begin(title);
    ImGui::SliderFloat("Speed max", &_speed, 0.01f, 0.2f);
    ImGui::SliderFloat("Fov", &_fov, 0.f, 180.f);
    ImGui::SliderFloat("Separation Distance", &_separation, 0.0f, 5.0f);
    ImGui::SliderFloat("Separation Factor", &_separation_factor, 0.1f, 1.f);
    ImGui::SliderFloat("Alignement Distance", &_alignment , 0.0f, 5.0f);
    ImGui::SliderFloat("Alignement Factor", &_align_factor, 0.1f, 1.f);
    ImGui::SliderFloat("Cohesion Distance"  , &_cohesion  , 0.0f, 5.0f);
    ImGui::SliderFloat("Cohesion Factor", &_cohesion_factor, 0.1f, 1.f);
    ImGui::SliderFloat("Follow factor", &_follow_factor, 0.f, 0.005f);
    ImGui::SliderFloat("Predator factor", &_predator_factor, 0.f, 1.f);
    ImGui::SliderFloat("Predator distance", &_predator_dist, 0.f, 5.f);

    ImGui::End();
    _angle = radians(_fov);
}

void Boids::update_uniforms(vec3 &follow, vec3 &predator)
{
    _program.use();
    _program.addUniformFloat(_bound, "bound");
    _program.addUniformFloat(_speed, "speed_max");
    _program.addUniformFloat(_angle, "angle_max");
    _program.addUniformFloat(_separation, "separation");
    _program.addUniformFloat(_alignment, "alignment");
    _program.addUniformFloat(_cohesion, "cohesion");
    _program.addUniformFloat(_separation_factor, "separation_factor");
    _program.addUniformFloat(_align_factor, "align_factor");
    _program.addUniformFloat(_cohesion_factor, "cohesion_factor");
    _program.addUniformFloat(_follow_factor, "follow_factor");
    _program.addUniformVec3(follow, "follow_obj");
    _program.addUniformFloat(_predator_factor, "predator_factor");
    _program.addUniformVec3(predator, "predator_obj");
    _program.addUniformFloat(_predator_dist, "predator_dist");
}

void Boids::draw(program &p)
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _mat_ssbo);
    _model.draw_instances(p, _size);
}
