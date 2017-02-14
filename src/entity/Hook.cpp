#include "Hook.h"
#include "glm/gtx/quaternion.hpp"

using namespace physx;


const float SCALE = 0.5;
const float BASE_RAD = 4.0;
const float BASE_ROT = 0;
const float HEIGHT_MOD = -0.8;

Hook::Hook(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents) :
	PhysicsObject(model_fname, tex_fname, actor, physicsManager),
    chain(new HookChain("assets/models/cylinder/cylinder.obj", "assets/models/sphere/blue.png")) {
    ents.push_back(chain.get());

	//Record the angle of shot?
    actor->userData = this;
	actor->setName("Hook");
    mStuck = false;
    mShot = false;
    actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
    this->scale(0.5, 0.5, 0.5);
}

void Hook::applyGlobalForce(glm::vec3 direction, double magnitude) {
	PxVec3 physVec(direction.x, direction.y, direction.z);
	PxRigidBodyExt::addForceAtLocalPos(*mActor, physVec * magnitude, PxVec3(0, 0, 0));
}

void Hook::applyLocalForce(float forward, float right, float up) {
	PxVec3 physVec(right, up, -forward);
	PxRigidBodyExt::addLocalForceAtLocalPos(*mActor, physVec, PxVec3(0, 0, 0));
}

void Hook::update(glm::vec3 carPos) {
    if (mShot && !mStuck) {
        mActor->setLinearVelocity(200.f*PxVec3(dir.x, dir.y, dir.z));
    }

    updatePosandRot();

    // Reposition/set renderable for connecting chain
    if (mShot) {
        chain->enable(true);
        chain->reposition(carPos, pos);
    }
    else {
        chain->enable(false);
    }
}