#ifndef CAMERA_HH
#define CAMERA_HH

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
    Camera(glm::vec3 position, glm::vec3 target, glm::vec3 up)
        : _position(position)
        , _target(target)
        , _up(up)
    {}

    glm::mat4 look_at();

    glm::vec3 get_position();
    glm::vec3 get_target();
    glm::vec3 get_up();

    void set_position(glm::vec3 position);
    void set_target(glm::vec3 target);
    void set_up(glm::vec3 up);

private:
    glm::vec3 _position;
    glm::vec3 _target;
    glm::vec3 _up;
};

#endif /* CAMERA_HH */
