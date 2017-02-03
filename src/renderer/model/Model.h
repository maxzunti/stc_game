#pragma once
#include "../GLUtil.h"
// Open Asset Import Library includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// Class to store the raw data (and vbo array, once initialized) of a loaded model
class Model {
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    void copy_ai_data(const aiMesh* mesh, const std::string &fname);

public:
    std::vector<unsigned int> indices; // make me private

    // Stores internal OpenGL pointers to each section of GPU vertex memory
    GLuint vbo[VBO::COUNT];
    GLuint vao[VAO::COUNT];

    Model();
    Model(std::string fname);
    Model(std::vector<glm::vec3> points,
        std::vector<glm::vec2> uvs,
        std::vector<glm::vec3> normals,
        std::vector<unsigned int> indices);

    ~Model();

    bool load_model_from_file(const std::string &fname);
};