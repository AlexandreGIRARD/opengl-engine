#ifndef PROGRAM_HH
#define PROGRAM_HH

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <cmath>
#include <string>

class program
{
public:
    program();
    ~program();
    void add_shader(std::string path, int shader_type);
    void link();
    void use();
    void addUniformTexture(const uint unit, const char *name);
    void addUniformMat4(glm::mat4 &matrix, const char *name);
    void addUniformVec3(const glm::vec3 vector, const char *name);
    void addUniformFloat(float val, const char* name);

private:
        uint _id_program;
        std::vector<uint> _id_shaders;
};

#endif /* PROGRAM_HH */
