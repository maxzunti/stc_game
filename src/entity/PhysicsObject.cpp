#include "PhysicsObject.h"
#include <iostream>
#include "glm/gtc/quaternion.hpp"

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
    setPos(mActor->getGlobalPose().p.x,
        mActor->getGlobalPose().p.y,
        mActor->getGlobalPose().p.z);

    PxQuat &pQuat = mActor->getGlobalPose().q;
    glm::vec3 rot = glm::eulerAngles(glm::tquat<double>(pQuat.w, pQuat.x, pQuat.y, pQuat.z));
    setRot(rot);
}