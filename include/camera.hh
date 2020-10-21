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
    Camera(vec3 position, vec3 target, vec3 up, float fov, float speed, float near, float far, float ratio)
        : _position(position)
        , _target(target)
        , _up(up)
        , _forward(target - position)
        , _right(- cross(up, _forward))
        , _speed(speed)
        , _fov(fov)
        , _yaw(90.f)
        , _pitch(0.f)
        , _near(near)
        , _far(far)
    {
        _projection = perspective(radians(_fov), ratio, _near, _far);
    }

    Camera(vec3 position, vec3 target, vec3 up)
        : _position(position)
        , _target(target)
        , _up(up)
        , _forward(target - position)
        , _right(- cross(up, _forward))
        , _speed(0)
        , _fov(90.f)
        , _yaw(90.f)
        , _pitch(0.f)
        , _near(1)
        , _far(10)
    {}

    mat4 look_at();

    float get_speed();
    vec3 get_position();
    vec3 get_target();
    vec3 get_up();
    vec3 get_forward();
    vec3 get_right();
    mat4 get_projection();

    void set_speed(float speed);
    void set_position(vec3 position);
    void set_position_y(float y);
    void set_target(vec3 target);
    void set_up(vec3 up);
    void set_forward(vec3 forward);
    void set_right(vec3 right);

    void update(GLFWwindow *window, float delta, float mouse_x, float mouse_y);
    void mouse_move(double xpos, double ypos);
    void invert_pitch();

private:
    vec3 _position;
    vec3 _target;
    vec3 _up;
    vec3 _forward;
    vec3 _right;

    float _speed;
    float _fov;
    float _yaw;
    float _pitch;
    float _near;
    float _far;

    mat4 _projection;

    vec2 _last_mouse_pos;
    vec2 _mouse_pos;
    bool _first_move = true;
    float _sensitivity = 0.1f;
};

#endif /* CAMERA_HH */
