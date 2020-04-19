#ifndef LIGHT_HH
#define LIGHT_HH

#include <glm/glm.hpp>

#include "program.hh"
#include "camera.hh"

using namespace glm;

enum light_type
{
    DIRECTIONAL = 0,
    POINT = 1
};

class Light
{
public:
    Light(light_type type, vec3 pos, vec3 color, vec3 intensity);
    uint set_shadow_framebuffer();
    void draw_shadow_map();
    void setup_program(vec3 direction, vec3 optional_pos);
    uint get_light_id();

    static uint _nb_lights;

private:
    light_type _type;
    vec3 _pos;
    vec3 _color;
    vec3 _intensity;
    uint _id;
    program _program;
};

uint Light::_nb_lights = 0;
#endif /* LIGHT_HH */
