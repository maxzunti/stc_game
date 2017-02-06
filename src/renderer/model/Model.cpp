#include "Model.h"
#include <iostream>

Model::Model() { // delete me
}

Model::Model(std::string model_fname, std::string tex_fname) :
    tex(new Texture(tex_fname.c_str())) {
    glGenBuffers(VBO::COUNT, vbo);
    glGenBuffers(VBO::COUNT, vbo);

    glGenVertexArrays(VAO::COUNT, vao);
    initVAO(vao, vbo);
    load_model_from_file(model_fname);
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

void Model::copy_ai_data(const aiMesh* mesh, const std::string &fname)
{
    // Clear old data
    points.clear();
    normals.clear();
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
            uvs.push_back(glm::vec2(m_vec.x, m_vec.y));
        }
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

bool Model::load_model_from_file(const std::string& fname)
{
    Assimp::Importer importer;

    // calcs tangents, joins vertices, triangulates, gens UVs and normals if necessary
    // (see http://assimp.sourceforge.net/lib_html/postprocess_8h.html for reference)
    const aiScene* scene = importer.ReadFile(fname, aiProcessPreset_TargetRealtime_Fast | aiProcess_GenUVCoords);

    if (!scene) {
        std::cout << "Error loading " << fname << ": \n" << importer.GetErrorString() << std::endl;
        return false;
    }

    if (scene->mNumMeshes != 1) {
        std::cout << "Error: file " << fname << " contains " << scene->mNumMeshes << " meshes.";
        return false;
    }

    // Mesh successfully found
    copy_ai_data(scene->mMeshes[0], fname);
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
}

void Model::scale(double &x_scl, double &y_scl, double &z_scl) {
    scaling = scaling * glm::mat4(
        glm::vec4(x_scl, 0, 0, 0),
        glm::vec4(0, y_scl, 0, 0),
        glm::vec4(0, 0, z_scl, 0),
        glm::vec4(0, 0, 0, 1));
}