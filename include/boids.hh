#pragma once

#include "model.hh"
#include "program.hh"

typedef struct boid
{
    vec3 pos;
    vec3 dir;
} boid_t;

using shared_model = std::shared_ptr<Model>;
using swarm = std::vector<boid_t>;


class Boids
{
public:
    Boids()
    {}
    Boids(int size, float speed, float separation, float alignment, float cohesion, shared_model model);
    void check_bound(int i);
    vec3 get_align_vect(swarm &local_swarm);
    vec3 get_center_swarm(swarm &local_swarm);
    void update();
    void draw(program p);

private:
    int _size;
    float _speed;
    float _separation, _alignment, _cohesion;
    std::vector<boid_t> _swarm;
    shared_model _model;
};
