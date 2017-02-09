#pragma once
#include "PhysicsObject.h"

class Hook : public PhysicsObject {
protected:

public:
	Hook::Hook(std::string model_fname, std::string tex_fname, PxRigidBody* actor, float angle);

	virtual void update();

	virtual void applyGlobalForce(glm::vec3 direction, double magnitude);
	virtual void applyLocalForce(float forward, float right, float up);
};