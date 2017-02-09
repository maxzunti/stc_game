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
	PxPhysics* mPhysics;
	PxCooking* mCooking;
	PxTolerancesScale mTolerancesScale;
	PxDefaultCpuDispatcher* mDispatcher;
	PxScene* mScene;
	PxMaterial* mMaterial;
    PxVisualDebuggerConnection* gConnection = NULL;
    VehicleSceneQueryData*	mVehicleSceneQueryData = NULL;
    PxBatchQuery*			mBatchQuery = NULL;

    PxVehicleDrivableSurfaceToTireFrictionPairs* mFrictionPairs = NULL;

    PxRigidStatic*			mGroundPlane = NULL;
    PxVehicleDrive4W*		mVehicle4W = NULL;

    bool					mIsVehicleInAir = true;

public:
	PhysicsManager();
	~PhysicsManager();
	PxActor* PhysicsManager::createGroundPlane();
	PxRigidBody* PhysicsManager::createBlock(float,float,float);
	void PhysicsManager::stepPhysics();
};

