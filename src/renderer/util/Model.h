#pragma once
#include "GLUtil.h"

// Class to store the raw data (and vbo array, once initialized) of a loaded model
class Model {
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

public:
    // Stores internal OpenGL pointers to each section of GPU vertex memory
    GLuint vbo[VBO::COUNT];
    GLuint vao[VAO::COUNT];

    Model(std::vector<glm::vec3> points,
        std::vector<glm::vec2> uvs,
        std::vector<glm::vec3> normals,
        std::vector<unsigned int> indices);

    ~Model();

    bool load_model_from_file(const std::string &fname);
};