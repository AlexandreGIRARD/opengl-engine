#include <camera.hh>

glm::mat4 Camera::look_at()
{
    return glm::lookAt(_position, _target, _up);
}

glm::vec3 Camera::get_position()
{
    return _position;
}

glm::vec3 Camera::get_target()
{
    return _target;
}

glm::vec3 Camera::get_up()
{
    return _up;
}

void Camera::set_position(glm::vec3 position)
{
    _position = position;
}

void Camera::set_target(glm::vec3 target)
{
    _target = target;
}

void Camera::set_up(glm::vec3 up)
{
    _up = up;
}
