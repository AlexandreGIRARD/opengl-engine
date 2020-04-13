#ifndef MODEL_HH
#define MODEL_HH

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <program.hh>
#include <mesh.hh>

#include <string>

class Model
{
public:
    Model(std::string path, mat4 &model, vec3 diffuse, vec3 specular, float shininess)
    {
        load_model(path);
        _model = model;
        _diffuse = diffuse;
        _specular = specular;
        _shininess = shininess;
    }
    void draw(program &p);
    mat4 get_model();
    void set_model(mat4 &model);

private:
    void load_model(std::string path);
    void process_node(aiNode *node, const aiScene *scene);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Mesh> _meshes;
    std::string _dir;

    mat4 _model;
    vec3 _diffuse;
    vec3 _specular;
    float _shininess;
};

#endif /* MODEL_HH */
