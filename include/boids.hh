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


class Boids : public Model
{
public:
    Boids(std::string path, std::shared_ptr<Material> mat,int size, float speed,
        float fov, float separation, float alignment, float cohesion);
    void check_bound(int i);
    void align_boid(int i, swarm &local_swarm);
    void center_boid(int i, swarm &local_swarm);
    void separe_boid(int i, swarm &local_swarm);
    void clamp_speed(int i);
    bool in_field_of_view(boid_t &b1, boid_t &b2);
    void update_ui();
    void update(std::vector<std::shared_ptr<Boids>> swarms);
    void draw(program &p) override;

private:
    static int _nb_swarms;
    int _id;
    int _size;
    float _fov;
    float _speed;
    float _angle;
    float _separation, _alignment, _cohesion;
    float _separation_factor = 0.05f;
    float _align_factor = 0.05f;
    float _cohesion_factor = 0.005f;
    std::vector<boid_t> _swarm;
};
