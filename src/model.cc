#include <glm/glm.hpp>

#include <model.hh>

#include <iostream>

using namespace glm;

Model::Model(std::string path, mat4 &model, std::shared_ptr<Material> mat)
{
    load_model(path);
    _model = model;
    _mat = mat;
}

Model::Model(std::string path, std::shared_ptr<Material> mat, vec3 translation, vec3 rotation, vec3 scale, float degree)
{
    load_model(path);
    _mat = mat;
    _translation = translation;
    _rotation = rotation;
    _scale = scale;
    _degree = degree;
    compute_model();
}

void Model::draw(program &p)
{
    _mat->set_uniforms(p);
    p.addUniformMat4(_model, "model");
    for (auto mesh : _meshes)
        mesh.draw(p);
}

void Model::draw_patches(program &p)
{
    _mat->set_uniforms(p);
    p.addUniformMat4(_model, "model");
    for (auto mesh : _meshes)
        mesh.draw_patches(p);
}

void Model::load_model(std::string path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs
        | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "Import model error" << importer.GetErrorString() << "\n";
        return;
    }
    _dir = path.substr(0, path.find_last_of('/'));

    process_node(scene->mRootNode, scene);
}


void Model::process_node(aiNode *node, const aiScene *scene)
{
    for (uint i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        _meshes.emplace_back(process_mesh(mesh, scene));
    }

    for (uint i = 0; i < node->mNumChildren; i++)
        process_node(node->mChildren[i], scene);
}

Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint> indices;

    for (uint i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.position = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        vertex.uv = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        vertex.tangent = vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

        vertices.emplace_back(vertex);
    }

    for (uint i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (uint j = 0; j < face.mNumIndices; j++)
            indices.emplace_back(face.mIndices[j]);
    }

    return Mesh(vertices, indices, _dir);
}

mat4 Model::get_model()
{
    return _model;
}

void Model::set_model(mat4 &model)
{
    _model = model;
}

void Model::compute_model()
{
    mat4 tmp_model = mat4(1.0);
    tmp_model = translate(tmp_model, _translation);
    tmp_model = rotate(tmp_model, radians(_degree), _rotation);
    _model = scale(tmp_model, _scale);
}
