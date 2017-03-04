#pragma once
#include "PhysicsObject.h"
#include "PxPhysicsAPI.h"

class Obstacle : public PhysicsObject
{
public:
	Obstacle(std::string model_fname, std::string tex_fname, physx::PxRigidBody* actor, glm::vec3 scales, PhysicsManager* physicsManager);


};

