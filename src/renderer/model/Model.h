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
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec2> base_uvs;
    std::unique_ptr<Texture> tex;

    glm::vec2 uv_scale = glm::vec2(1.0f, 1.0f);

    bool tex_loaded = false;
    bool is_loaded = false;
    bool draw_sil = true;

    void copy_ai_data(const aiMesh* mesh, const std::string &fname);

    glm::mat4 scaling;
public:
    // Stores internal OpenGL pointers to each section of GPU vertex memory
    GLuint vbo[VBO::COUNT];
    GLuint vao[VAO::COUNT];

    Model();
    Model(std::string model_fname, std::string tex_fname, int mesh_id = 0);
    Model(std::vector<glm::vec3> points,
        std::vector<glm::vec2> uvs,
        std::vector<glm::vec3> normals,
        std::vector<unsigned int> indices);

    ~Model();
    std::vector<glm::vec3> points;
    std::vector<unsigned int> indices;
    std::vector<glm::vec3>* getPoints();
    std::vector<unsigned int>* getIndices();
    bool load_model_from_file(const std::string &fname, int mesh_id);
    const Texture * getTex() const;
    bool is_tex_loaded() const;
    const int num_indices() const;

    const glm::mat4& get_scaling() const;
    void reset_scale();
    void scale(double &x_scl, double &y_scl, double &z_scl);

    // used to tile textures
    void tile_UV_Y(double factor);
    void tile_UV_X(double factor);
    void scroll_UV_Y(float offset);
    void scroll_UV_X(float offset);

    // model_fname is expected to contain multiple meshes, but still correspond to a single UV-mapped texture (used for all meshes)
    static std::vector<Model*> load_multimesh_models(std::string model_fname, std::string tex_fname);

    // Skybox reflectivity
    float SkyR = 0.3f;

    // Model-scaling normalization factors
    double x_norm = 1.;
    double y_norm = 1.;
    double z_norm = 1.;

    static const float DEFAULT_SIL;
    double sil_x = DEFAULT_SIL;
    double sil_y = DEFAULT_SIL;
    double sil_z = DEFAULT_SIL;
    double sil_jitter = 0.0f;

    void drawSil(bool);
    bool shouldSil() const;
};