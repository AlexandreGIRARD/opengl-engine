#include <iostream>
#include <sstream>

#include <light.hh>

uint Light::_nb_lights = 0;

Light::Light(vec3 color, float intensity)
{
    _color = color;
    _intensity = intensity;
    _id = _nb_lights;
    _nb_lights += 1;
}

uint Light::get_light_id()
{
    return _id;
}

uint Light::get_map()
{
    return _map;
}

mat4 Light::get_view()
{
    return _view;
}

mat4 Light::get_projection()
{
    return _projection;
}

program Light::get_program()
{
    return _program;
}
