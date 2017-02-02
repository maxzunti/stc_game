#include "Model.h"
#include <iostream>

// Open Asset Import Library includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Model::Model(std::vector<glm::vec3> points,
    std::vector<glm::vec2> uvs,
    std::vector<glm::vec3> normals,
    std::vector<unsigned int> indices)
{
    this->points = points;
    this->normals = normals;
    this->uvs = uvs;
    this->indices = indices;

    glGenBuffers(VBO::COUNT, vbo);
    glGenVertexArrays(VAO::COUNT, vao);
    initVAO(vao, vbo);
    loadBuffer(vbo, points, normals, uvs, indices);
}

Model::~Model()
{
    // Delete OpenGL buffers
    glDeleteVertexArrays(VAO::COUNT, vao);
    glDeleteBuffers(VBO::COUNT, vbo);
}


bool Model::load_model_from_file(const std::string& fname)
{
    // Create an instance of the Importer class
    Assimp::Importer importer;
    return true;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll 
    // propably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile(fname,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices
        );
}