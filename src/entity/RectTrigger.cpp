#include "RectTrigger.h"

RectTrigger::RectTrigger(PhysicsManager* physicsManager, std::string texName, float x_width, float y_width, float z_width, int id, bool renderable) :
    StaticPhysicsObject("assets/models/Crate/Crate1.obj", texName, glm::vec3(x_width,y_width,z_width), physicsManager,COLLISION_FLAG_TRIGGER,COLLISION_FLAG_TRIGGER_AGAINST) {
  //  mActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
    this->renderable = renderable;
    PxShape* shapes[1]; // expect a single shape per trigger volume
    mActor->getShapes(shapes, 1);
    mActor->detachShape(*shapes[0]);
    physicsManager->mScene->removeActor(*mActor);
    PxMaterial* mat = this->mPhysicsManager->mMaterial;
    // Make a new shape
    shapes[0] = mActor->createShape(PxBoxGeometry(x_width, y_width, z_width), &mat, 1);
    shapes[0]->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
    shapes[0]->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

    mActor->setName("LapBox");
    mActor->userData = this;
    this->triggerID = id;
    physicsManager->mScene->addActor(*mActor);
}

RectTrigger::~RectTrigger()
{
}
