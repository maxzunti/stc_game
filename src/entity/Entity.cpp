#include "Entity.h"
#include <cstring>

Entity::Entity() { }

void Entity::getPos(double buf[3]) {
	memcpy(buf, pos, 3*sizeof(double));
}

double Entity::xPos() { return pos[0]; }

double Entity::yPos() { return pos[1]; }

double Entity::zPos() { return pos[2]; }

double Entity::xRot() { return rot[0]; }

double Entity::yRot() { return rot[1]; }

double Entity::zRot() { return rot[2]; }