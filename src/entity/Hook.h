#pragma once
#include "PhysicsObject.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleFilterShader.h"

class Hook : public PhysicsObject {
protected:

public:
	Hook(std::string model_fname, std::string tex_fname, physx::PxRigidBody* actor, PhysicsManager* physicsManager);

	virtual void update();

	virtual void applyGlobalForce(glm::vec3 direction, double magnitude);
	virtual void applyLocalForce(float forward, float right, float up);
};