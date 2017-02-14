#include "Entity.h"
#include <cstring>
#include <glm/gtx/quaternion.hpp>

Entity::Entity() { }

const glm::vec3& Entity::getPos() const {
    return pos;
}

glm::quat Entity::getQRot() const {
    return qrot;
}

glm::vec3 Entity::getDir() const {
    return dir;
}


void Entity::setPos(double x, double y, double z) {
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

void Entity::setPos(glm::vec3 newPos) {
    pos = newPos;
}

void Entity::setRot(double x, double y, double z) {
    glm::vec3 euler(x, y, z);
    qrot = glm::quat(euler);
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));
}

void Entity::setRot(glm::vec3 newRot) {
    qrot = glm::quat(newRot);
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));
}

void Entity::setRot(glm::quat newRot) {
    qrot = newRot;
}

void Entity::rotate(double x, double y, double z) {
    glm::vec3 euler(x, y, z);
    glm::quat nquat(euler);

    qrot = nquat * qrot;
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));
}

double Entity::xPos() const { return pos[0]; }

double Entity::yPos() const { return pos[1]; }

double Entity::zPos() const { return pos[2]; }

bool Entity::canRender() { return false; }
