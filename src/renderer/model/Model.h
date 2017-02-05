#pragma once
#include "../GLUtil.h"
#include "Texture.h"
// Open Asset Import Library includes
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <memory>

// Class to store the raw data (and vbo array, once initialized) of a loaded model
class Model {
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::unique_ptr<Texture> tex;
    std::vector<unsigned int> indices;

    bool tex_loaded = false;
    bool is_loaded = false;

    void copy_ai_data(const aiMesh* mesh, const std::string &fname);

    glm::mat4 scaling;
public:
    // Stores internal OpenGL pointers to each section of GPU vertex memory
    GLuint vbo[VBO::COUNT];
    GLuint vao[VAO::COUNT];

    Model();
    Model(std::string model_fname, std::string tex_fname);
    Model(std::vector<glm::vec3> points,
        std::vector<glm::vec2> uvs,
        std::vector<glm::vec3> normals,
        std::vector<unsigned int> indices);

    ~Model();

    bool load_model_from_file(const std::string &fname);
    const Texture * getTex() const;
    bool is_tex_loaded() const;
    const int num_indices() const;

    const glm::mat4& get_scaling() const;
    void reset_scale();
    void scale(double &x_scl, double &y_scl, double &z_scl);
};