#include "boids.hh"

#include <random>

Boids::Boids(int size, float speed, float separation, float alignment, float cohesion, shared_model model)
    : _size(size)
    , _speed(speed)
    , _separation(separation)
    , _alignement(alignment)
    , _cohesion(cohesion)
    , _model(model)
{
    // Init random generator
    std::uniform_real_distribution<float> dis_bound(-10.f, 10.f);
    std::uniform_real_distribution<float> dis(-1.f, 1.f);
    std::random_device gen;

    // Initialize random boids
    boids = std::vector<boid_t>(size);
    for (int i = 0; i < _size; i++)
    {
        boids[i].pos = vec3(dis_bound(gen), dis_bound(gen), dis_bound(gen));
        boids[i].dir = vec3(dis(gen), dis(gen), dis(gen));
    }
}


void Boids::draw(program p)
{
    for (int i = 0; i < _size; i++)
    {
        mat4 trans_mat = translate(mat4(1.0), boids[i].pos);
        mat4 rotat_mat = lookAt(boids[i].pos, boids[i].pos + boids[i].dir, vec3(0, 1, 0));
        // rotat_mat = mat4(1.0);
        mat4 model_mat = trans_mat * rotat_mat;
        _model->set_model(model_mat);
        _model->draw(p);
    }
}
