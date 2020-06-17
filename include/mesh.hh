#ifndef MESH_HH
#define MESH_HH

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <program.hh>

#include <vector>
#include <string>

using namespace glm;

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 uv;
};

class Mesh
{
public:
    Mesh(std::vector<Vertex> &vertices, std::vector<uint> &indices, std::string path);

    void draw(program p);
    uint load_texture(const char *path);

    std::vector<uint> _indices;
    std::vector<Vertex> _vertices;


private:
    void setup_mesh();

    uint _VAO, _VBO, _EBO;
};

#endif /* MESH_HH */
