#include "boids.hh"

#include <random>
#include <glm/gtx/vector_angle.hpp>

#include <iostream>


Boids::Boids(int size, float speed, float separation, float alignment, float cohesion, shared_model model)
    : _size(size)
    , _speed(speed)
    , _separation(separation)
    , _alignment(alignment)
    , _cohesion(cohesion)
    , _model(model)
{
    // Init random generator
    std::uniform_real_distribution<float> dis_bound(-5.f, 5.f);
    std::uniform_real_distribution<float> dis(-1.f, 1.f);
    std::random_device gen;

    // Initialize random boids
    _swarm = std::vector<boid_t>(size);
    for (int i = 0; i < _size; i++)
    {
        _swarm[i].pos = vec3(dis_bound(gen), dis_bound(gen), dis_bound(gen));
        _swarm[i].dir = 0.1f * normalize(vec3(dis(gen), dis(gen), dis(gen)));
    }
}

// void Boids::check_bound(int i)
// {
//     vec3 normal = vec3(0.f);
//     if (_swarm[i].pos.x >= 10.f)
//         normal = vec3(-1.f, 0.f, 0.f);
//     if (_swarm[i].pos.x <= -10.f)
//         normal = vec3(1.f, 0.f, 0.f);
//     if (_swarm[i].pos.y >= 10.f)
//         normal = vec3(0.f, -1.f, 0.f);
//     if (_swarm[i].pos.y <= -10.f)
//         normal = vec3(0.f, 1.f, 0.f);
//     if (_swarm[i].pos.z >= 10.f)
//         normal = vec3(0.f, 0.f, -1.f);
//     if (_swarm[i].pos.z <= -10.f)
//         normal = vec3(0.f, 0.f, 1.f);
//     if (normal != vec3(0.f))
//     {
//         _swarm[i].dir = reflect(_swarm[i].dir, normal);
//         float d = 0.3 - dot(_swarm[i].pos +10.f * normal, normal);
//         _swarm[i].pos += d* normal;
//     }
// }

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
    const float align_factor = 0.05f;
    vec3 align = vec3(0.f);
    for (auto boid : local_swarm)
        align += boid.dir;
    if (local_swarm.size())
        align /= local_swarm.size();
    _swarm[i].dir += (align - _swarm[i].dir) * align_factor;
}

// Center boids with neighbors
void Boids::center_boid(int i, swarm &local_swarm)
{
    const float center_factor = 0.005f;
    vec3 center = vec3(0.f);
    for (auto boid : local_swarm)
        center += boid.pos;
    if (local_swarm.size())
        center /= local_swarm.size();
    _swarm[i].dir += (center - _swarm[i].pos) * center_factor;
}

// Separe current boid from others
void Boids::separe_boid(int i, swarm &local_swarm)
{
    const float avoid_factor = 0.05f;
    vec3 move = vec3(0.f);
    for (auto boid : local_swarm)
        move += _swarm[i].pos - boid.pos;
    _swarm[i].dir += move * avoid_factor;
}

void Boids::clamp_speed(int i)
{
    const float speed_max = 0.1f; // TODO: parse in init
    const float speed = length(_swarm[i].dir);
    if (speed >= speed_max)
        _swarm[i].dir = (_swarm[i].dir / speed) * speed_max;
}

void Boids::update()
{
    for (int i = 0; i < _size; i++)
    {
        auto separation_swarm = swarm();
        auto alignment_swarm = swarm();
        auto cohesion_swarm = swarm();

        for (int j = 0; j < _size; j++)
        {
            if (i == j)
                continue;
            // TODO: Check not in view

            float dist = distance(_swarm[i].pos, _swarm[j].pos);
            if (dist <= _separation)
                separation_swarm.emplace_back(_swarm[j]);
            if (dist <= _alignment)
                alignment_swarm.emplace_back(_swarm[j]);
            if (dist <= _cohesion)
                cohesion_swarm.emplace_back(_swarm[j]);
        }

        center_boid(i, cohesion_swarm);
        align_boid(i, alignment_swarm);
        separe_boid(i, separation_swarm);
        clamp_speed(i);

        // _swarm[i].dir = normalize(separation_vect + alignment_vect + cohesion_vect);
        _swarm[i].pos += _swarm[i].dir;
        check_bound(i);
    }
}


void Boids::draw(program p)
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
        mat4 model_mat = trans_mat * rotat_mat;
        _model->set_model(model_mat);
        _model->draw(p);
    }
}
