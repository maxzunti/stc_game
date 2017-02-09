#include "AimArrow.h"

AimArrow::AimArrow(std::string model_fname, std::string tex_fname) : 
    Renderable(model_fname, tex_fname) {
    scale(SCALE, SCALE, SCALE);
}

void AimArrow::reposition(glm::vec3 up, glm::vec3 pos, glm::vec3 aim_vec, glm::quat aim_rot) {
    this->pos = pos + (BASE_RAD * aim_vec) + (HEIGHT_MOD * up);
    this->qrot = glm::rotate(aim_rot, BASE_ROT, up);
    int x = 5;
}