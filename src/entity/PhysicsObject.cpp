#include "PhysicsObject.h"
#include <iostream>

PhysicsObject::PhysicsObject(std::string filename, PxActor* act) 
	: Renderable(filename)
{
	mActor = act;
}

PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::updatePos()
{
	double buf[3] = { mActor->getWorldBounds().getCenter().x, mActor->getWorldBounds().getCenter().y,
		mActor->getWorldBounds().getCenter().z };
	this->setPos(buf);
}