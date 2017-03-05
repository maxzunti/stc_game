#include "StaticPhysicsObject.h"
#include "glm/gtx/quaternion.hpp"

#define M_PI 3.14159265358979323846
using namespace physx;

StaticPhysicsObject::StaticPhysicsObject(std::string model_fname, std::string tex_fname, PhysicsManager* physicsManager)

    : Renderable(model_fname, tex_fname)
{
    mPhysicsManager = physicsManager;
    mActor = mPhysicsManager->createTriangleMesh(this->getModels().at(0), false, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
    mPhysicsManager->mScene->addActor(*mActor);
}

StaticPhysicsObject::~StaticPhysicsObject()
{
}

void StaticPhysicsObject::update() {
    updatePosandRot();
}

void StaticPhysicsObject::updatePosandRot()
{
    setPos(mActor->getGlobalPose().p.x,
        mActor->getGlobalPose().p.y,
        mActor->getGlobalPose().p.z);

    PxQuat &pQuat = mActor->getGlobalPose().q;
    setRot(glm::quat(pQuat.w, pQuat.x, pQuat.y, pQuat.z));
}

void StaticPhysicsObject::setPos(double x, double y, double z) {
    pos = glm::vec3(x, y, z);
    PxVec3 newPos(x, y, z);
    mActor->setGlobalPose(PxTransform(newPos, mActor->getGlobalPose().q));
}

void StaticPhysicsObject::setPos(glm::vec3 &nPos) {
    pos = nPos;
    PxVec3 newPos(nPos.x, nPos.y, nPos.z);
    mActor->setGlobalPose(PxTransform(newPos, mActor->getGlobalPose().q));
}

void StaticPhysicsObject::setRot(double x, double y, double z) {
    glm::vec3 euler(x, y, z);

    qrot = glm::tquat<double>(euler);
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));

    PxQuat newRot(qrot.x, qrot.y, qrot.z, qrot.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}

void StaticPhysicsObject::setRot(glm::vec3 &nRot) {
    qrot = glm::quat(nRot);
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));

    PxQuat newRot(qrot.x, qrot.y, qrot.z, qrot.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}

void StaticPhysicsObject::setRot(glm::quat &nRot) {
    qrot = nRot;
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));

    PxQuat newRot(qrot.x, qrot.y, qrot.z, qrot.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}


// Apply rotations in radians
void StaticPhysicsObject::rotate(double x, double y, double z) {
    glm::vec3 euler(x, y, z);
    glm::quat nquat(euler);

    qrot = nquat * qrot;
    dir = glm::rotate(qrot, glm::vec3(0, 0, -1));

    PxQuat newRot(qrot.x, qrot.y, qrot.z, qrot.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}