#include "Model.h"
#include <iostream>

const float Model::DEFAULT_SIL = 1.1;

Model::Model() { // delete me

}

Model::Model(std::string model_fname, std::string tex_fname, int mesh_id) :
    tex(new Texture(tex_fname.c_str())) {
    glGenBuffers(VBO::COUNT, vbo);
    glGenBuffers(VBO::COUNT, vbo);

    glGenVertexArrays(VAO::COUNT, vao);
    initVAO(vao, vbo);
    load_model_from_file(model_fname, mesh_id);
    is_loaded = true;
    if (tex)
        tex_loaded = true;
    reset_scale();
}

Model::Model(std::vector<glm::vec3> points,
             std::vector<glm::vec2> uvs,
             std::vector<glm::vec3> normals,
             std::vector<unsigned int> indices)
{
    this->points = points;
    this->normals = normals;
    this->uvs = uvs;
    this->base_uvs = uvs;
    this->indices = indices;

    glGenBuffers(VBO::COUNT, vbo);
    glGenVertexArrays(VAO::COUNT, vao);
    initVAO(vao, vbo);
    loadBuffer(vbo, points, normals, uvs, indices);
    reset_scale();
}

Model::~Model()
{
    // Delete OpenGL buffers
    glDeleteVertexArrays(VAO::COUNT, vao);
    glDeleteBuffers(VBO::COUNT, vbo);
}

void Model::copy_ai_data(const aiMesh* mesh, const std::string &fname)
{
    // Clear old data
    points.clear();
    normals.clear();
    base_uvs.clear();
    uvs.clear();
    indices.clear();

    // Load vertices
    for (int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D m_vec = mesh->mVertices[i];
        points.push_back(glm::vec3(m_vec.x, m_vec.y, m_vec.z));
    }

    // Load normals
    for (int i = 0; i < mesh->mNumVertices; i++) {
        aiVector3D m_vec = mesh->mNormals[i];
        normals.push_back(glm::vec3(m_vec.x, m_vec.y, m_vec.z));
    }

    // (Try to ) load uvs
    if (mesh->mNumUVComponents[0] > 0) {
        for (int i = 0; i < mesh->mNumVertices; i++) {
            aiVector3D m_vec = mesh->mTextureCoords[0][i];
            base_uvs.push_back(glm::vec2(m_vec.x, m_vec.y));
        }
        uvs = base_uvs;
    } else {
        std::cout << "Warning: " << fname << " has no UV coordinates." << std::endl;
    }

    // Load indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        assert(face.mNumIndices == 3); // should've been taken care of by 'triangulate' in ReadFile macro arg
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }
}

std::vector<glm::vec3>* Model::getPoints() {
    return &this->points;
}
std::vector<unsigned int>* Model::getIndices() {
    return &this->indices;
}


bool Model::load_model_from_file(const std::string& fname, int mesh_id = 0)
{
    Assimp::Importer importer;

    // calcs tangents, joins vertices, triangulates, gens UVs and normals if necessary
    // (see http://assimp.sourceforge.net/lib_html/postprocess_8h.html for reference)
    const aiScene* scene = importer.ReadFile(fname, aiProcessPreset_TargetRealtime_Fast | aiProcess_GenUVCoords);

    if (!scene) {
        std::cout << "Error loading " << fname << ": \n" << importer.GetErrorString() << std::endl;
        return false;
    }

    std::cout << "loading model " << fname << std::endl;

    // Mesh successfully found
    copy_ai_data(scene->mMeshes[mesh_id], fname);
    loadBuffer(vbo, points, normals, uvs, indices);
    return true;
}

const int Model::num_indices() const {
    return indices.size();
}

const Texture * Model::getTex() const {
    return tex.get();
}

bool Model::is_tex_loaded() const { return tex_loaded; }

const glm::mat4& Model::get_scaling() const {
    return scaling;
}

void Model::reset_scale() {
    scaling = glm::mat4(
        glm::vec4(1, 0, 0, 0),
        glm::vec4(0, 1, 0, 0),
        glm::vec4(0, 0, 1, 0),
        glm::vec4(0, 0, 0, 1));
    scale(x_norm, y_norm, z_norm);
}

void Model::scale(double &x_scl, double &y_scl, double &z_scl) {
    scaling = scaling * glm::mat4(
        glm::vec4(x_scl, 0, 0, 0),
        glm::vec4(0, y_scl, 0, 0),
        glm::vec4(0, 0, z_scl, 0),
        glm::vec4(0, 0, 0, 1));
}

void Model::tile_UV_Y(double factor) {
    std::vector<glm::vec2> newUVs;;

    for (int i = 0; i < uvs.size(); i++) {
        glm::vec2 coords = uvs.at(i);
        coords.y *= factor;
        newUVs.push_back(std::move(coords));
    }

    // Somewhat inefficient to reupload everything - potentially optimize
    loadBuffer(vbo, points, normals, newUVs, indices);
}

void Model::scroll_UV_Y(float offset) {
    for (int i = 0; i < uvs.size(); i++) {
        uvs[i].y += offset;
    }
}

void Model::scroll_UV_X(float offset) {
    for (int i = 0; i < uvs.size(); i++) {
        uvs[i].x += offset;
    }
}


// Generalized static function which loads multi-mesh models sharing a texture (e.g. 'testcar.obj')
std::vector<Model*> Model::load_multimesh_models(std::string model_fname, std::string tex_fname) {

    Assimp::Importer importer;
    std::vector<Model*> loaded_models;

    // calcs tangents, joins vertices, triangulates, gens UVs and normals if necessary
    // (see http://assimp.sourceforge.net/lib_html/postprocess_8h.html for reference)
    const aiScene* scene = importer.ReadFile(model_fname, aiProcessPreset_TargetRealtime_Fast | aiProcess_GenUVCoords);

    if (!scene) {
        std::cout << "Error loading " << model_fname << ": \n" << importer.GetErrorString() << std::endl;
        return loaded_models;
    }
    
    for (int i = 0; i < scene->mNumMeshes; i++) {
        loaded_models.emplace_back(new Model(model_fname, tex_fname, i));
    }
    return loaded_models;
}