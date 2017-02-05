#include "PhysicsObject.h"
#include <iostream>

PhysicsObject::PhysicsObject(std::string model_fname, std::string tex_fname, PxActor* act)
	: Renderable(model_fname, tex_fname)
{
	mActor = act;
}

PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::updatePos()
{
    this->setPos(mActor->getWorldBounds().getCenter().x,
                 mActor->getWorldBounds().getCenter().y,
                 mActor->getWorldBounds().getCenter().z);
}