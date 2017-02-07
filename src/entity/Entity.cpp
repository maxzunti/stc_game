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

void Entity::setPos(glm::vec3 &newPos) {
    rot = std::move(newPos); // yoink
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

double Entity::xPos() const { return pos[0]; }

double Entity::yPos() const { return pos[1]; }

double Entity::zPos() const { return pos[2]; }

double Entity::xRot() const { return rot[0]; }

double Entity::yRot() const { return rot[1]; }

double Entity::zRot() const { return rot[2]; }

bool Entity::canRender() { return false; }
