#include "PhysicsObject.h"
#include <iostream>
#include "glm/gtx/quaternion.hpp"


#define M_PI 3.14159265358979323846
using namespace physx;

PhysicsObject::PhysicsObject(std::string model_fname, std::string tex_fname, PxRigidBody* act, PhysicsManager* physicsManager)

	: Renderable(model_fname, tex_fname)
{
	mActor = act;
    mPhysicsManager = physicsManager;
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
    qrot = glm::tquat<double>(pQuat.w, pQuat.x, pQuat.y, pQuat.z);
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
    glm::vec3 euler(x, y, z);

    qrot = glm::tquat<double>(euler);
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));

    PxQuat newRot(qrot.x, qrot.y, qrot.z, qrot.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}

void PhysicsObject::setRot(glm::vec3 &nRot) {
    qrot = glm::quat(nRot);
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));

    PxQuat newRot(qrot.x, qrot.y, qrot.z, qrot.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}

void PhysicsObject::setRot(glm::quat &nRot) {
    qrot = nRot;
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));

    PxQuat newRot(qrot.x, qrot.y, qrot.z, qrot.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}


// Apply rotations in radians
void PhysicsObject::rotate(double x, double y, double z) {
    glm::vec3 euler(x, y, z);
    glm::quat nquat(euler);

    qrot = nquat * qrot;
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));

    PxQuat newRot(qrot.x, qrot.y, qrot.z, qrot.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}