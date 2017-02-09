#include "AimArrow.h"

AimArrow::AimArrow(std::string model_fname, std::string tex_fname) : 
    Renderable(model_fname, tex_fname) {
    scale(SCALE, SCALE, SCALE);
}

void AimArrow::reposition(glm::vec3 pos, glm::quat aim) {
    this->pos = pos;
    this->pos.y += 2.0;
    this->qrot = aim;
}