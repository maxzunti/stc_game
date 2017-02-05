#pragma once
#include "Renderable.h"
#include "../physics/PhysicsManager.h"

class PhysicsObject : public Renderable {
protected:

public:
	PhysicsObject(std::string filename, PxActor* actor);
	~PhysicsObject();
	void updatePos();
	PxActor* mActor;
};