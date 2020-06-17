#include <camera.hh>

glm::mat4 Camera::look_at()
{
    return glm::lookAt(_position, _position + _forward, _up);
}

float Camera::get_speed()
{
    return _speed;
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

glm::vec3 Camera::get_right()
{
    return _right;
}

void Camera::set_speed(float speed)
{
    _speed = speed;
}

void Camera::set_position(glm::vec3 position)
{
    _position = position;
}

void Camera::set_position_y(float y)
{
    _position.y = y;
}

void Camera::set_target(glm::vec3 target)
{
    _target = target;
}

void Camera::set_up(glm::vec3 up)
{
    _up = up;
}

void Camera::set_forward(glm::vec3 forward)
{
    _forward = forward;
}

void Camera::set_right(glm::vec3 right)
{
    _right = right;
}

void Camera::update(GLFWwindow *window, float delta, float mouse_x, float mouse_y)
{
    // Keyboard events
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)  // W : forwards
    {
        set_position(get_position() + get_forward() * get_speed() * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  // S : backwards
    {
        set_position(get_position() - get_forward() * get_speed() * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  // A : left
    {
        set_position(get_position() - get_right() * get_speed() * delta);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)  // D : right
    {
        set_position(get_position() + get_right() * get_speed() * delta);
    }

    // Mouse events
    mouse_move(mouse_x, mouse_y);
}

void Camera::mouse_move(double xpos, double ypos)
{
    _mouse_pos.x = xpos;
    _mouse_pos.y = ypos;

    if (_first_move)
    {
        _last_mouse_pos = vec2(xpos, ypos);
        _first_move = false;
    }

    float offset_x = xpos - _last_mouse_pos.x;
    float offset_y = _last_mouse_pos.y - ypos;

    _last_mouse_pos = vec2(xpos, ypos);

    offset_x *= _sensitivity;
    offset_y *= _sensitivity;

    _yaw   += offset_x;
    _pitch += offset_y;

    if(_pitch > 89.0f)
        _pitch = 89.0f;
    if(_pitch < -89.0f)
        _pitch = -89.0f;

    _forward.x = cos(radians(_yaw)) * cos(radians(_pitch));
    _forward.y = sin(radians(_pitch));
    _forward.z = sin(radians(_yaw)) * cos(radians(_pitch));
    _forward = normalize(_forward);
    _right = normalize(cross(_forward, vec3(0.f, 1.f, 0.f)));
    _up = normalize(cross(_right, _forward));
}

void Camera::invert_pitch()
{
    _pitch = -_pitch;
    _forward.x = cos(radians(_yaw)) * cos(radians(_pitch));
    _forward.y = sin(radians(_pitch));
    _forward.z = sin(radians(_yaw)) * cos(radians(_pitch));
    _forward = normalize(_forward);
    _right = normalize(cross(_forward, vec3(0.f, 1.f, 0.f)));
    _up = normalize(cross(_right, _forward));
}
