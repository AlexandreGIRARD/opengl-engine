#ifndef CAMERA_HH
#define CAMERA_HH

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

class Camera
{
public:

    Camera()
    {}
    Camera(vec3 position, vec3 target, vec3 up)
        : _position(position)
        , _target(target)
        , _up(up)
    {}

    mat4 look_at();

    vec3 get_position();
    vec3 get_target();
    vec3 get_up();

    void set_position(vec3 position);
    void set_target(vec3 target);
    void set_up(vec3 up);

private:
    vec3 _position;
    vec3 _target;
    vec3 _up;
};

#endif /* CAMERA_HH */
