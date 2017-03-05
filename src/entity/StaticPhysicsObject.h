#pragma once
#include "Renderable.h"
#include "PxPhysicsAPI.h"
#include "../physics/PhysicsManager.h"

class StaticPhysicsObject : public Renderable {
protected:

public:

    physx::PxRigidStatic* mActor;
    PhysicsManager* mPhysicsManager;

    StaticPhysicsObject::StaticPhysicsObject(std::string model_fname, std::string tex_fname, PhysicsManager* physicsManager);

    StaticPhysicsObject::~StaticPhysicsObject();

    void updatePosandRot();
    virtual void update();

    // Overrides to interact with PhysX
    virtual void setPos(double x, double y, double z);
    virtual void setPos(glm::vec3 &newPos);
    virtual void setRot(double x, double y, double z);
    virtual void setRot(glm::vec3 &newRot);
    virtual void setRot(glm::quat &newRot);
    virtual void rotate(double x, double y, double z);
};