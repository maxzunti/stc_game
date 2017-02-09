#pragma once
#include "PxPhysicsAPI.h"
#include <iostream>

using namespace physx;

class PhysicsManager
{

	PxFoundation* mFoundation;
	PxProfileZoneManager* mProfileZoneManager;
	PxPhysics* mPhysics;
	PxCooking* mCooking;
	PxTolerancesScale mTolerancesScale;
	PxDefaultCpuDispatcher* mDispatcher;
	PxScene* mScene;
	PxMaterial* mMaterial;

public:
	PhysicsManager();
	~PhysicsManager();
	PxActor* PhysicsManager::createGroundPlane();
	PxRigidBody* PhysicsManager::createBlock(float,float,float);
	void PhysicsManager::stepPhysics();
};

