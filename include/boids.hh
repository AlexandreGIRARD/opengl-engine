#pragma once

#include "model.hh"
#include "program.hh"

using shared_model = std::shared_ptr<Model>;

typedef struct boid
{
    vec3 pos;
    vec3 dir;
} boid_t;

class Boids
{
public:
    Boids()
    {}
    Boids(int size, float speed, float separation, float alignment, float cohesion, shared_model model);
    void draw(program p);

private:
    int _size;
    float _speed;
    float _separation, _alignement, _cohesion;
    std::vector<boid_t> boids;
    shared_model _model;
};
