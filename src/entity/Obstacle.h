#pragma once
#include "DynamicPhysicsObject.h"
#include "PxPhysicsAPI.h"

class Obstacle : public DynamicPhysicsObject
{
public:
	Obstacle(std::string model_fname, std::string tex_fname, physx::PxRigidBody* actor, glm::vec3 scales, PhysicsManager* physicsManager);


};

