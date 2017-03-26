#include "Hook.h"
#include "glm/gtx/quaternion.hpp"

using namespace physx;


const float SCALE = 0.5;
const float BASE_RAD = 4.0;
const float BASE_ROT = 0;
const float HEIGHT_MOD = -0.8;

Hook::Hook(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents) :
	DynamicPhysicsObject(model_fname, tex_fname, actor, physicsManager),
    chain(new HookChain("assets/models/cylinder/cylinder2.obj", "assets/models/cylinder/bline.png")),
    hook_parser("config/hook_config", &hookParams)
{
    ents.push_back(chain.get());
    initHookParams();
    updateFromConfig();
    // At this point, we've loaded a single hook model using the PhysicsObject constructor
    assert(models.size() == 1);
    unattached = models[0];
    attached = new Model(model_fname, "assets/models/AimArrow/red.png");

	//Record the angle of shot?
    actor->userData = this;
	actor->setName("Hook");
    mStuck = false;
    mShot = false;
    actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
    this->scale(0.5, 0.5, 0.5);
}

Hook::~Hook() {
    // Delete whichever model isn't in 'models' (since Renderable's destructor will catch anything that is)
    std::cout << "Hook destructor" << std::endl;
    if (mStuck)
        delete unattached;
    else
        delete attached;
    this->chain.release();
}

void Hook::initHookParams() {
    hookParams.push_back(std::make_pair(std::string("HOOK_SPEED"), &HOOK_SPEED));
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
        mActor->setLinearVelocity(HOOK_SPEED*PxVec3(dir.x, dir.y, dir.z));
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

void Hook::setStuck(bool val) {
    mStuck = val;
    chain->setStuck(mStuck);

    // Update active model
    if (mStuck) {
        models[0] = attached;
    }
    else {
        models[0] = unattached;
    }
}

void Hook::setShot(bool val) {
    mShot = val;
}

bool Hook::getStuck() {
    return mStuck;
}

bool Hook::getShot() {
    return mShot;
}

void Hook::updateFromConfig() {
    hook_parser.updateFromFile();
}
