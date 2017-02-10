#pragma once
#include "PxPhysicsAPI.h"
#include <iostream>
#include "vehicle/PxVehicleUtil.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleRaycast.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleTireFriction.h"

using namespace physx;

class PhysicsManager
{

	PxFoundation* mFoundation;
	PxProfileZoneManager* mProfileZoneManager;
	PxTolerancesScale mTolerancesScale;
	PxDefaultCpuDispatcher* mDispatcher;
	
    PxVisualDebuggerConnection* gConnection = NULL;
    VehicleSceneQueryData*	mVehicleSceneQueryData = NULL;
    PxBatchQuery*			mBatchQuery = NULL;

    PxVehicleDrivableSurfaceToTireFrictionPairs* mFrictionPairs = NULL;

    bool					mIsVehicleInAir = true;

public:

    PxScene* mScene;
    PxPhysics* mPhysics;
    PxCooking* mCooking;
    PxMaterial* mMaterial;

	PhysicsManager();
	~PhysicsManager();
	PxActor* PhysicsManager::createGroundPlane();
	PxActor* PhysicsManager::createWallPlane(int, int, int, int, int);
	PxRigidBody* PhysicsManager::createBlock(float,float,float);
	void PhysicsManager::stepPhysics();
};

