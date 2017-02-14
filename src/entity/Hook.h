#pragma once
#include "PhysicsObject.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleFilterShader.h"
#include "HookChain.h"

class Hook : public PhysicsObject {
protected:

public:
	Hook(std::string model_fname, std::string tex_fname, physx::PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents);
    std::unique_ptr<HookChain> chain;
    bool mStuck;
    bool mShot;
	virtual void update(glm::vec3 carPos);

	virtual void applyGlobalForce(glm::vec3 direction, double magnitude);
	virtual void applyLocalForce(float forward, float right, float up);
};