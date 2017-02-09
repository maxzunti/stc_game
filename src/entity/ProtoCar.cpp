#include "ProtoCar.h"
#include <glm/gtx/quaternion.hpp>
#include <math.h>

using namespace glm;

ProtoCar::ProtoCar(std::string model_fname, std::string tex_fname, PxRigidBody* actor, Input* cont, std::vector<Entity*> &ents) :
    PhysicsObject(model_fname, tex_fname, actor),
    arrow(new AimArrow("assets/models/AimArrow/AimArrow.obj", "assets/models/AimArrow/red.png"))
{
    controller = cont;
    ents.push_back(arrow.get());
}

void ProtoCar::applyGlobalForce(glm::vec3 direction, double magnitude) {
    PxVec3 physVec(direction.x, direction.y, direction.z);
    PxRigidBodyExt::addForceAtLocalPos(*mActor, physVec * magnitude, PxVec3(0, 0, 0));
}

void ProtoCar::applyLocalForce(float forward, float right, float up) {
    PxVec3 physVec(forward, up, right); // currently crossing these over
    PxRigidBodyExt::addLocalForceAtLocalPos(*mActor, physVec, PxVec3(0, 0, 0));
}

void ProtoCar::calcAim() {
    vec3 right = normalize(cross(dir, vec3(0, 1, 0)));
    up = normalize(cross(right, dir));
    float angle = 0.0f;
    if (!controller->RStick_InDeadzone()) {
        angle = atan(controller->RightStick_Y() /
                     controller->RightStick_X()) - (M_PI / 2);
        if (controller->RightStick_X() < 0) { // Stick in left half
            angle -= M_PI;
        }
    }
    aim_rot = glm::rotate(qrot, angle, up);
    aim = glm::rotate(aim_rot, vec3(0, 0, -1));
}

void ProtoCar::update() {
    // Basic movement and rotation - this'll be heavily modified in the real car
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

    // Perform physX update
    updatePosandRot();

    // Update aim (after PhysX!)
    calcAim();
    arrow->reposition(up, pos, aim, aim_rot);
}

glm::vec3 ProtoCar::getAim() const {
    return aim;
}

