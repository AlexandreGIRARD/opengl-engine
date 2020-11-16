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
    Boids(int size, float speed, float fov, float separation, float alignment, float cohesion, shared_model model);
    void check_bound(int i);
    void align_boid(int i, swarm &local_swarm);
    void center_boid(int i, swarm &local_swarm);
    void separe_boid(int i, swarm &local_swarm);
    void clamp_speed(int i);
    bool in_field_of_view(boid_t &b1, boid_t &b2);
    void update(std::vector<std::shared_ptr<Boids>> swarms);
    void draw(program p);

private:
    static int _nb_swarms;
    int _id;
    int _size;
    float _speed;
    float _angle;
    float _separation, _alignment, _cohesion;
    std::vector<boid_t> _swarm;
    shared_model _model;
};
