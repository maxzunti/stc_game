#include "PhysicsObject.h"
#include <iostream>

PhysicsObject::PhysicsObject(std::string model_fname, std::string tex_fname, PxRigidBody* act)
	: Renderable(model_fname, tex_fname)
{
	mActor = act;
}

PhysicsObject::~PhysicsObject()
{
}

void PhysicsObject::updatePosandRot()
{
    this->setPos(mActor->getGlobalPose().p.x,
                 mActor->getGlobalPose().p.y,
                 mActor->getGlobalPose().p.z);
	PxReal xrot, yrot, zrot;
	PxQuat temp = mActor->getGlobalPose().q;
	temp.toRadiansAndUnitAxis(xrot, PxVec3(1, 0, 0));
	temp.toRadiansAndUnitAxis(yrot, PxVec3(0, 1, 0));
	temp.toRadiansAndUnitAxis(zrot, PxVec3(0, 0, 1));	

	this->setRot(xrot, yrot, zrot);
}