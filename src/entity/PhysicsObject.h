#pragma once
#include "Renderable.h"
#include "../physics/PhysicsManager.h"

class PhysicsObject : public Renderable {
protected:

public:
	PhysicsObject(std::string model_fname, std::string tex_fname, PxActor* actor);
	~PhysicsObject();
	void updatePos();
	PxActor* mActor;
};