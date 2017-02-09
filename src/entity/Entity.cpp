#include "Entity.h"
#include <cstring>

Entity::Entity() { }

const glm::vec3& Entity::getPos() const {
    return pos;
}

void Entity::setPos(double x, double y, double z) {
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

void Entity::setPos(glm::vec3 newPos) {
    pos = newPos;
}

glm::tquat<double> Entity::getQRot() const {
    return qrot;
}

void Entity::setRot(double x, double y, double z) {
    glm::vec3 euler(x, y, z);
    qrot = glm::tquat<double>(euler);
}

void Entity::setRot(glm::vec3 newRot) {
    qrot = glm::tquat<double>(newRot);
}

double Entity::xPos() const { return pos[0]; }

double Entity::yPos() const { return pos[1]; }

double Entity::zPos() const { return pos[2]; }

bool Entity::canRender() { return false; }
