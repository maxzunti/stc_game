#include "RectTrigger.h"

RectTrigger::RectTrigger(PhysicsManager* physicsManager, std::string texName, float x_width, float y_width, float z_width, bool renderable) :
    StaticPhysicsObject("assets/models/cube/cube.obj", texName, physicsManager) {
  //  mActor->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
    this->renderable = renderable;
    PxShape* shapes[1]; // expect a single shape per trigger volume
    mActor->getShapes(shapes, 1);
    mActor->detachShape(*shapes[0]);
    physicsManager->mScene->removeActor(*mActor);

    // Make a new shape
    PxMaterial * junkMat = physicsManager->mPhysics->createMaterial(0.3, 0.3, 0.3);
    mActor->createShape(PxBoxGeometry(x_width, y_width, z_width), &junkMat, 1, PxShapeFlag::eTRIGGER_SHAPE);
    physicsManager->mScene->addActor(*mActor);

    scale(x_width, y_width, z_width);
}