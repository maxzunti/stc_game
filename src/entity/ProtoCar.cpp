#include "ProtoCar.h"

ProtoCar::ProtoCar(std::string model_fname, std::string tex_fname, PxRigidBody* actor, Input* cont, std::vector<Entity*> &ents) :
    PhysicsObject(model_fname, tex_fname, actor),
    reticle(new Renderable("assets/models/reticle/reticle.obj", "assets/models/Crate/crate_1.jpg"))
{
    controller = cont;
    ents.push_back(reticle.get());
    reticle->scale(2.5, 2.5, 2.5);
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
    if (pos.y <= 1.01 && controller->GetButtonPressed(XButtonIDs::A)) {
        applyLocalForce(0, 0, 2000);
    }
    if (controller->GetButtonPressed(XButtonIDs::L_Shoulder)) {
        rotate(0., 0.05, 0.);
    }
    if (controller->GetButtonPressed(XButtonIDs::R_Shoulder)) {
        rotate(0., -0.05, 0.);
    }

    updatePosandRot();

    glm::vec3 retPos = pos;
    retPos.y = pos.y - 1.0;
    reticle->setPos(retPos);
}