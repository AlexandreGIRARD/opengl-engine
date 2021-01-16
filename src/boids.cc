#include "boids.hh"

#include <random>
#include <cmath>
#include <glm/gtx/vector_angle.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

int Boids::_nb_swarms = 0;

Boids::Boids(std::string path, std::shared_ptr<Material> mat, int size, float speed, float fov, float separation, float alignment, float cohesion)
    : Model(path, mat)
    , _size(size)
    , _fov(fov)
    , _speed(speed)
    , _angle(radians(fov))
    , _separation(separation)
    , _alignment(alignment)
    , _cohesion(cohesion)
{
    _id = _nb_swarms++;
    std::cout << _id << std::endl;
    // Init random generator
    std::uniform_real_distribution<float> dis_bound(-8.f, 8.f);
    std::uniform_real_distribution<float> dis(-1.f, 1.f);
    std::random_device gen;

    // Initialize random boids
    _swarm = std::vector<boid_t>(size);
    auto positions  = std::vector<vec4>(size);
    auto directions = std::vector<vec4>(size);
    for (int i = 0; i < _size; i++)
    {
        _swarm[i].pos = vec3(dis_bound(gen), dis_bound(gen), dis_bound(gen));
        // positions[i]  = vec4(dis_bound(gen), dis_bound(gen), dis_bound(gen), 0);
        positions[i] = vec4(i,i,i,0);
        _swarm[i].dir = normalize(vec3(dis(gen), dis(gen), dis(gen)));
        // directions[i] = normalize(vec4(dis(gen), dis(gen), dis(gen), 0));
        directions[i] = vec4(i,i,i,0);
    }

    //TODO: init boids draw program
    // Create program for rendering reflection texture
    _draw_program = program();
    _draw_program.add_shader("boids/boids.vs.glsl", GL_VERTEX_SHADER);
    _draw_program.add_shader("boids/boids.gs.glsl", GL_GEOMETRY_SHADER);
    _draw_program.add_shader("boids/boids.fs.glsl", GL_FRAGMENT_SHADER);
    _draw_program.link();

    _comp_program = program();
    _comp_program.add_shader("boids/boids.cs.glsl", GL_COMPUTE_SHADER);
    _comp_program.link();

    //TODO: Init SSBO boids
    _comp_program.use();
    glGenBuffers(1, &_pos_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _pos_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _size * sizeof(vec4), positions.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _pos_ssbo); // unbind
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &_dir_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _dir_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _size * sizeof(vec4), directions.data(), GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _dir_ssbo); // unbind
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &_mat_ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _mat_ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _size * sizeof(mat4), NULL, GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _mat_ssbo); // unbind
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    _comp_program.addUniformUint(_swarm.size(), "max_boids");

    std::cout << _swarm[0].pos.x << " " << _swarm[0].pos.y << _swarm[0].pos.z << "\n" << _swarm[0].dir.x << " " << _swarm[0].dir.y << _swarm[0].dir.z << "\n";
}

void Boids::update() // GPU Version
{
    _comp_program.use();
    glDispatchCompute(ceil(_swarm.size() / 1024.f), 1, 1);
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Boids::check_bound(int i)
{
    vec3 center_sphere = vec3(0.f);
    if (distance(_swarm[i].pos, center_sphere) < 10.f)
        return;
    vec3 normal = normalize(center_sphere - _swarm[i].pos);
    vec3 direction =  _swarm[i].dir - dot(_swarm[i].dir, normal) * normal;
    vec3 friction =  dot(_swarm[i].dir, normal) * normal;
    // _swarm[i].dir = normalize(0.75f * direction - 0.0f * friction);
    _swarm[i].dir = reflect(_swarm[i].dir, normal);
    while(distance(_swarm[i].pos, center_sphere) >= 10.f)
        _swarm[i].pos += 0.05f*normal;
}

// Align boids with neighbors
void Boids::align_boid(int i, swarm &local_swarm)
{
    vec3 align = vec3(0.f);
    for (auto boid : local_swarm)
        align += boid.dir;
    if (local_swarm.size())
        align /= local_swarm.size();
    _swarm[i].dir += (align) * _align_factor;
}

// Center boids with neighbors
void Boids::center_boid(int i, swarm &local_swarm)
{
    vec3 center = vec3(0.f);
    for (auto boid : local_swarm)
        center += boid.pos;
    if (local_swarm.size())
        center /= local_swarm.size();
    _swarm[i].dir += (center - _swarm[i].pos) * _cohesion_factor;
}

// Separe current boid from others
void Boids::separe_boid(int i, swarm &local_swarm)
{
    vec3 move = vec3(0.f);
    for (auto boid : local_swarm)
        move += _swarm[i].pos - boid.pos;
    if (local_swarm.size())
        move /= local_swarm.size();
    _swarm[i].dir += move * _separation_factor;
}

void Boids::clamp_speed(int i)
{
    const float speed = length(_swarm[i].dir);
    if (speed >= _speed)
        _swarm[i].dir = (_swarm[i].dir / speed) * _speed;
}

bool Boids::in_field_of_view(boid_t &b1, boid_t &b2)
{
    vec3 b1_view = normalize(b1.dir);
    vec3 b1_to_b2 = normalize(b2.pos - b1.pos);
    return glm::angle(b1_to_b2, b1_view) <= _angle;
}

void Boids::update_ui()
{
    const auto title = ("Swarm " + std::to_string(_id)).c_str();
    ImGui::Begin(title);
    ImGui::SliderFloat("Speed max", &_speed, 0.0f, 0.2f);
    ImGui::SliderFloat("Fov", &_fov, 0.f, 180.f);
    ImGui::SliderFloat("Separation Distance", &_separation, 0.0f, 5.0f);
    ImGui::SliderFloat("Separation Factor", &_separation_factor, 0.01f, 0.1f);
    ImGui::SliderFloat("Alignement Distance", &_alignment , 0.0f, 5.0f);
    ImGui::SliderFloat("Alignement Factor", &_align_factor, 0.01f, 0.1f);
    ImGui::SliderFloat("Cohesion Distance"  , &_cohesion  , 0.0f, 5.0f);
    ImGui::SliderFloat("Cohesion Factor", &_cohesion_factor, 0.001f, 0.01f);
    ImGui::End();
    _angle = radians(_fov);
}

void Boids::update(std::vector<std::shared_ptr<Boids>> swarms)
{
    update_ui();

    for (int i = 0; i < _size; i++)
    {
        for (auto swarm : swarms)
        {
            if (swarm->_id == _id)
                continue;
            auto separation_swarm = std::vector<boid_t>();
            for (int j = 0; j < _size; j++)
            {
                float dist = distance(_swarm[i].pos, swarm->_swarm[j].pos);
                if (in_field_of_view(_swarm[i], swarm->_swarm[j]) && dist <= _separation)
                    separation_swarm.emplace_back(swarm->_swarm[j]);
            }
            separe_boid(i, separation_swarm);
            clamp_speed(i);
            _swarm[i].pos += _swarm[i].dir;
            check_bound(i);
        }

        auto separation_swarm = std::vector<boid_t>();
        auto alignment_swarm = std::vector<boid_t>();
        auto cohesion_swarm = std::vector<boid_t>();

        for (int j = 0; j < _size; j++)
        {
            if (i == j || !in_field_of_view(_swarm[i], _swarm[j]))
                continue;
            // TODO: Check not in view

            float dist = distance(_swarm[i].pos, _swarm[j].pos);
            if (dist <= _separation)
                separation_swarm.emplace_back(_swarm[j]);
            else if (dist <= _alignment)
                alignment_swarm.emplace_back(_swarm[j]);
            else if (dist <= _cohesion)
                cohesion_swarm.emplace_back(_swarm[j]);
        }

        center_boid(i, cohesion_swarm);
        align_boid(i, alignment_swarm);
        separe_boid(i, separation_swarm);
        clamp_speed(i);

        _swarm[i].pos += _swarm[i].dir;
        check_bound(i);
    }
}


void Boids::draw(program &p)
{
    for (int i = 0; i < _size; i++)
    {
        // Generate translation matrix
        mat4 trans_mat = translate(mat4(1.0), _swarm[i].pos);
        // Generate rotatation matrix from ogirin firection (1,0,0) and dir vector
        vec3 normal_dir = normalize(_swarm[i].dir);
        float angle = glm::angle(vec3(0,0,1), normal_dir);
        vec3 axis = cross(vec3(0,0,1), normal_dir);
        mat4 rotat_mat = rotate(mat4(1.0), angle, axis);

        // Compute model matrix from M=T*R
        _model = trans_mat * rotat_mat;
        _mat->set_uniforms(p);
        p.addUniformMat4(_model, "model");
        for (auto mesh : _meshes)
            mesh.draw(p);
    }
}
