#include "ProtoCar.h"

ProtoCar::ProtoCar(std::string model_fname, std::string tex_fname, PxRigidBody* actor, Input* cont) :
    PhysicsObject(model_fname, tex_fname, actor) {
    controller = cont;
}

void ProtoCar::applyGlobalForce(glm::vec3 direction, double magnitude) {
    PxVec3 physVec(direction.x, direction.y, direction.z);
    PxRigidBodyExt::addForceAtLocalPos(*mActor, physVec * magnitude, PxVec3(0, 0, 0));
}

void ProtoCar::applyLocalForce(float forward, float right, float up) {
    PxVec3 physVec(forward, up, right); // currently crossing these over
    PxRigidBodyExt::addLocalForceAtLocalPos(*mActor, physVec, PxVec3(0, 0, 0));
}

void ProtoCar::update() {
    applyLocalForce(0, -(controller->RightTrigger() - controller->LeftTrigger()) * FORCE_FACTOR, 0);
    if (pos.y <= -0.999 && controller->GetButtonPressed(XButtonIDs::A)) {
        applyLocalForce(0, 0, 10000);
    }
    updatePosandRot();
}
