#include <camera.hh>

glm::mat4 Camera::look_at()
{
    return glm::lookAt(_position, _position + _forward, _up);
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

glm::vec3 Camera::get_forward()
{
    return _forward;
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

void Camera::update(GLFWwindow *window)
{
    float speed = 0.01;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        set_position(get_position() + get_forward() * speed);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        set_position(get_position() - get_forward() * speed);
    }
}
