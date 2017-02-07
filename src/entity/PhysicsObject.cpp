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

void PhysicsObject::update() {
    updatePosandRot();
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

void PhysicsObject::setPos(double x, double y, double z) {
    pos = glm::vec3(x, y, z);
    PxVec3 newPos(x, y, z);
    mActor->setGlobalPose(PxTransform(newPos, mActor->getGlobalPose().q));
}

void PhysicsObject::setPos(glm::vec3 &nPos) {
    pos = nPos;
    PxVec3 newPos(nPos.x, nPos.y, nPos.z);
    mActor->setGlobalPose(PxTransform(newPos, mActor->getGlobalPose().q));
}

void PhysicsObject::setRot(double x, double y, double z) {
    rot = glm::vec3(x, y, z);
    glm::tquat<double> nquat(rot);
    PxQuat newRot(nquat.x, nquat.y, nquat.z, nquat.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}

void PhysicsObject::setRot(glm::vec3 &nRot) {
    rot = nRot;
    glm::tquat<double> nquat(rot);
    PxQuat newRot(nquat.x, nquat.y, nquat.z, nquat.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}