#include "Trigger.h"

Trigger::Trigger(std::string model_fname, std::string tex_fname, PhysicsManager* physicsManager) :
    StaticPhysicsObject(model_fname, tex_fname, physicsManager) {
    mActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
    mActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
    PxShapeFlag::eTRIGGER_SHAPE
}