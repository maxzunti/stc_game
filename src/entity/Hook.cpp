#include "Hook.h"
#include "glm/gtx/quaternion.hpp"

using namespace physx;


const float SCALE = 0.5;
const float BASE_RAD = 4.0;
const float BASE_ROT = 0;
const float HEIGHT_MOD = -0.8;

Hook::Hook(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager) :
	PhysicsObject(model_fname, tex_fname, actor, physicsManager) {
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

void Hook::update() {
    if (mShot && !mStuck)
    {
        this->mActor->setLinearVelocity(30.f*PxVec3(this->getDir().x, this->getDir().y, this->getDir().z));
        
    }
    updatePosandRot();
}

void Hook::reposition(glm::vec3 up, glm::vec3 pos, glm::vec3 aim_vec, glm::quat aim_rot) {
    this->pos = pos + (BASE_RAD * aim_vec) + (HEIGHT_MOD * up);
    this->qrot = glm::rotate(aim_rot, BASE_ROT, up);
    int x = 5;
    
    dir = glm::rotate(this->qrot, glm::vec3(0, 0, -1));
    PxQuat newRot(this->qrot.x, this->qrot.y, this->qrot.z, this->qrot.w);
    mActor->setGlobalPose(PxTransform(mActor->getGlobalPose().p, newRot));
}