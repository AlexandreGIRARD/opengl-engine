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
    Model(std::string path, mat4 &model, std::shared_ptr<Material> mat)
    {
        load_model(path);
        _model = model;
        _mat = mat;
    }
    void draw(program &p);
    void draw_patches(program &p);
    mat4 get_model();
    void set_model(mat4 &model);

private:
    void load_model(std::string path);
    void process_node(aiNode *node, const aiScene *scene);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Mesh> _meshes;
    std::string _dir;

    mat4 _model;
    std::shared_ptr<Material> _mat;
};

#endif /* MODEL_HH */
