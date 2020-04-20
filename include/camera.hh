#ifndef CAMERA_HH
#define CAMERA_HH

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

using namespace glm;

class Camera
{
public:

    Camera()
    {}
    Camera(vec3 position, vec3 target, vec3 up)
        : _speed(1)
        , _position(position)
        , _target(target)
        , _up(up)
        , _forward(target - position)
        , _right(- cross(up, _forward))
    {}

    mat4 look_at();

    float get_speed();
    vec3 get_position();
    vec3 get_target();
    vec3 get_up();
    vec3 get_forward();
    vec3 get_right();

    void set_speed(float speed);
    void set_position(vec3 position);
    void set_target(vec3 target);
    void set_up(vec3 up);
    void set_forward(vec3 forward);
    void set_right(vec3 right);

    void update(GLFWwindow *window, float delta);

private:
    float _speed;
    vec3 _position;
    vec3 _target;
    vec3 _up;
    vec3 _forward;
    vec3 _right;
};

#endif /* CAMERA_HH */
