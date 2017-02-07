#pragma once
#include "../GLUtil.h"

class Skybox {
    std::string face_names[6];

    bool initSkyboxVAO();

public:
    GLuint vao, vbo, tex_id;
    Skybox(std::string files[6]);
};