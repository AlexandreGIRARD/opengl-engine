#ifndef MODEL_HH
#define MODEL_HH

#include <string>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "material.hh"
#include "program.hh"
#include "mesh.hh"


class Model
{
public:
    Model(std::string path, std::shared_ptr<Material> mat);
    Model(std::string path, mat4 &model, std::shared_ptr<Material> mat);
    Model(std::string path, std::shared_ptr<Material> mat, vec3 translation, vec3 rotation, vec3 scale, float degree);

    void draw(program &p);
    void draw_patches(program &p);
    mat4 get_model();
    void set_model(mat4 &model);
    void compute_model();

private:
    void load_model(std::string path);
    void process_node(aiNode *node, const aiScene *scene);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Mesh> _meshes;
    std::string _dir;

    vec3 _translation, _scale, _rotation;
    float _degree;
    mat4 _model;
    std::shared_ptr<Material> _mat;
};

#endif /* MODEL_HH */
