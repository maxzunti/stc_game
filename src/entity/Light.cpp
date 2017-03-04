#include "Light.h"

using namespace glm;


Light::Light(vec3 ipos, vec3 idir) {
    pos = ipos;
    dir = idir;
}

Light::~Light() {
}

glm::vec3& Light::getPos() {
    return pos;
}
glm::vec3 Light::getDir() {
    return dir;
}

void Light::setPos(double x, double y, double z) {
    pos = vec3(x, y, z);
}
void Light::setPos(glm::vec3 newPos) {
    pos = newPos;
}

void setPos(double x, double y, double z);
void setPos(glm::vec3 newPos);
void setRot(double x, double y, double z);
void setRot(glm::vec3 newRot);
void setRot(glm::quat newRot);
void rotate(double x, double y, double z);

