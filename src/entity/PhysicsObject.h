#pragma once
#include "Renderable.h"
#include "PxPhysicsAPI.h"
#include "../physics/PhysicsManager.h"

class PhysicsObject : public Renderable {
protected:

public:
	PhysicsObject(std::string model_fname, std::string tex_fname, physx::PxRigidBody* actor, PhysicsManager* physicsManager);
	~PhysicsObject();
	void updatePosandRot();
    virtual void update();
	physx::PxRigidBody* mActor;
    PhysicsManager* mPhysicsManager;

    // Overrides to interact with PhysX
    virtual void setPos(double x, double y, double z);
    virtual void setPos(glm::vec3 &newPos);
    virtual void setRot(double x, double y, double z);
    virtual void setRot(glm::vec3 &newRot);
    virtual void rotate(double x, double y, double z);
};