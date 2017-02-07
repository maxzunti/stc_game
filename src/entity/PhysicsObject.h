#pragma once
#include "Renderable.h"
#include "../physics/PhysicsManager.h"

class PhysicsObject : public Renderable {
protected:

public:
	PhysicsObject(std::string model_fname, std::string tex_fname, PxRigidBody* actor);
	~PhysicsObject();
	void updatePosandRot();
	PxRigidBody* mActor;

    // Overrides to interact with PhysX
    virtual void setPos(double x, double y, double z);
    virtual void setPos(glm::vec3 &newPos);
    virtual void setRot(double x, double y, double z);
    virtual void setRot(glm::vec3 &newRot);
};