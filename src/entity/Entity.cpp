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

const glm::vec3& Entity::getRot() const {
    return rot;
}

void Entity::setRot(double x, double y, double z) {
    rot.x = x;
    rot.y = y;
    rot.z = z;
}

void Entity::setRot(glm::vec3 &newRot) {
    rot = std::move(newRot); // yoink
}


double Entity::xPos() { return pos[0]; }

double Entity::yPos() { return pos[1]; }

double Entity::zPos() { return pos[2]; }

double Entity::xRot() { return rot[0]; }

double Entity::yRot() { return rot[1]; }

double Entity::zRot() { return rot[2]; }

bool Entity::canRender() { return false; }
