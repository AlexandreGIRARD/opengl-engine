#pragma one

#include <vector>
#include <memory>
#include <glm/glm.hpp>

#include "program.hh"
#include "camera.hh"
#include "model.hh"

using std::shared_ptr<Model> shared_model;
using std::shared_ptr<Light> shared_light;

class DeferredShading
{
public:
    void DeferredShading();
    void render(std::vector<shared_model> models);
    void bind_result();
private:
    void set_framebuffer();

    std::shared_ptr<Camera> _cam;

    program _program;


}
