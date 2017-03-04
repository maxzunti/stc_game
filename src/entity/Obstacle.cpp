#include "Obstacle.h"



Obstacle::Obstacle(std::string model_fname, std::string tex_fname, PxRigidBody* actor, glm::vec3 scales, PhysicsManager* physicsManager) :
	PhysicsObject(model_fname, tex_fname, actor, physicsManager) {

    this->SIL_X_SCALE = 1.01;
    this->SIL_Z_SCALE = 1.01;
    this->SIL_Y_SCALE = 1.01;
    scaleModels();

	this->scale(scales.x, scales.y, scales.z);

}
