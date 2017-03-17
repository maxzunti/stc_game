#pragma once
#include "Renderable.h"

class Wheel : public Renderable {
    glm::quat inverse_rot; // tracks the inverse rotation of the wheel; used for reflection

public:
    Wheel(std::string model_fname, std::string tex_fname);
    void set_inv_rot(glm::quat);
    glm::quat& get_inv_rot();
};