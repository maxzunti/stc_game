#pragma once
#include "Renderable.h"

class AimArrow : public Renderable {
public:
    const float SCALE = 0.5;
    const float RADIUS = 2.0;

    AimArrow(std::string model_fname, std::string tex_fname);
    void reposition(glm::vec3 pos, glm::quat aim);
};