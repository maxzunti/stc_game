#include "Obstacle.h"



Obstacle::Obstacle(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager) :
	PhysicsObject(model_fname, tex_fname, actor, physicsManager) {
	this->scale(100, 100, 100);
}

void Obstacle::update() {
	updatePosandRot();
}
