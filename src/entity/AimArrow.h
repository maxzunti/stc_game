#pragma once
#include "Renderable.h"

class AimArrow : public Renderable {
public:
    const float SCALE = 1.;
    const float BASE_RAD = 7.0;
    const float BASE_ROT = M_PI / 2;
    const float HEIGHT_MOD = -0.8;

    // TODO: Factors which determine how to resize the arrow based on its position from the car
    // const float RAD_FACTOR = 2.0f;
    // const float SCALE_FACTOR = 1.5;

    AimArrow(std::string model_fname, std::string tex_fname);
    void reposition(glm::vec3 up, glm::vec3 pos, glm::vec3 aim_vec, glm::quat aim_rot);
};