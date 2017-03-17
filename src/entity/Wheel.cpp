#include "Wheel.h"

Wheel::Wheel(std::string model_fname, std::string tex_fname) : 
    Renderable(model_fname, tex_fname) {

}

void Wheel::set_inv_rot(glm::quat nrot) {
    inverse_rot = nrot;
}

glm::quat& Wheel::get_inv_rot() {
    return inverse_rot;
}