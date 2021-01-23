#pragma once

#include "model.hh"
#include "program.hh"
#include "camera.hh"

typedef struct boid
{
    vec4 pos;
    vec4 dir;
} boid_t;

using shared_model = std::shared_ptr<Model>;
using swarm = std::vector<boid_t>;


class Boids
{
public:
    Boids(std::string path, std::shared_ptr<Material> mat,int size, float bound, float speed,
        float fov, float separation, float alignment, float cohesion);
    void update(vec3 &follow, vec3 &predator);
    void update_ui();
    void update_uniforms(vec3 &follow, vec3 &predator);
    void draw(program &p);

private:
    static int _nb_swarms;

    Model _model;
    program _program;
    uint _pos_ssbo;
    uint _dir_ssbo;
    uint _mat_ssbo;
    int _id;
    int _size;
    float _bound;
    float _fov;
    float _speed;
    float _angle;
    float _separation, _alignment, _cohesion;
    float _separation_factor = 0.05f;
    float _align_factor = 0.05f;
    float _cohesion_factor = 0.005f;
    float _follow_factor = 0.001;
    float _predator_factor = 0.1;
    float _predator_dist = 2.f;
};
