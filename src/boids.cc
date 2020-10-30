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
        _swarm[i].dir = normalize(vec3(dis(gen), dis(gen), dis(gen)));
    }
}

void Boids::check_bound(int i)
{
    vec3 normal = vec3(0.f);
    if (_swarm[i].pos.x >= 10.f)
        normal = vec3(-1.f, 0.f, 0.f);
    if (_swarm[i].pos.x <= -10.f)
        normal = vec3(1.f, 0.f, 0.f);
    if (_swarm[i].pos.y >= 10.f)
        normal = vec3(0.f, -1.f, 0.f);
    if (_swarm[i].pos.y <= -10.f)
        normal = vec3(0.f, 1.f, 0.f);
    if (_swarm[i].pos.z >= 10.f)
        normal = vec3(0.f, 0.f, -1.f);
    if (_swarm[i].pos.z <= -10.f)
        normal = vec3(0.f, 0.f, 1.f);
    if (normal != vec3(0.f))
    {
        _swarm[i].dir = reflect(_swarm[i].dir, normal);
        float d = 0.3 - dot(_swarm[i].pos +10.f * normal, normal);
        _swarm[i].pos += d* normal;
    }
}

vec3 Boids::get_align_vect(swarm &local_swarm)
{
    vec3 vect = vec3(0.f);
    for (auto boid : local_swarm)
        vect += boid.dir;
    if (local_swarm.size())
        vect /= local_swarm.size();
    return vect;
}
vec3 Boids::get_center_swarm(swarm &local_swarm)
{
    vec3 vect = vec3(0.f);
    for (auto boid : local_swarm)
        vect += boid.pos;
    if (local_swarm.size())
        vect /= local_swarm.size();
    return vect;
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
            else if (dist <= _alignment)
                alignment_swarm.emplace_back(_swarm[j]);
            else if (dist <= _cohesion)
                cohesion_swarm.emplace_back(_swarm[j]);
        }

        vec3 separation_vect = -get_center_swarm(separation_swarm);
        if (separation_vect != vec3(0, 0, 0))
            separation_vect = 0.06f * normalize(_swarm[i].pos - separation_vect);

        vec3 alignment_vect = normalize(_swarm[i].dir + get_align_vect(alignment_swarm));

        vec3 cohesion_vect = get_center_swarm(cohesion_swarm);
        if (cohesion_vect != vec3(0, 0, 0))
            cohesion_vect = 0.04f * normalize(cohesion_vect - _swarm[i].pos);

        _swarm[i].dir = normalize(separation_vect + alignment_vect + cohesion_vect);
        _swarm[i].pos += _speed * _swarm[i].dir;
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
        float angle = glm::angle(vec3(0,0,1), _swarm[i].dir);
        vec3 axis = cross(vec3(0,0,1), _swarm[i].dir);
        mat4 rotat_mat = rotate(mat4(1.0), angle, axis);

        // Compute model matrix from M=T*R
        mat4 model_mat = trans_mat * rotat_mat;
        _model->set_model(model_mat);
        _model->draw(p);
    }
}
