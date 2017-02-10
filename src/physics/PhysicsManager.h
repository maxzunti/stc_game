#pragma once
#include "PxPhysicsAPI.h"
#include <iostream>
#include <memory>
#include "vehicle/PxVehicleUtil.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleRaycast.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleTireFriction.h"


class PhysicsManager
{

	PxFoundation* mFoundation;
	PxProfileZoneManager* mProfileZoneManager;
	PxTolerancesScale mTolerancesScale;
	PxDefaultCpuDispatcher* mDispatcher;
    PxVisualDebuggerConnection* gConnection = NULL;

    bool mIsVehicleInAir = true;

public:

    PxScene* mScene;
    PxPhysics* mPhysics;
    PxCooking* mCooking;
    PxMaterial* mMaterial;
    VehicleSceneQueryData*	mVehicleSceneQueryData = NULL;
    PxBatchQuery*			mBatchQuery = NULL;
    PxVehicleDrivableSurfaceToTireFrictionPairs* mFrictionPairs = NULL;

	PhysicsManager();
	~PhysicsManager();
	PxActor* PhysicsManager::createGroundPlane();
	PxRigidBody* PhysicsManager::createBlock(float,float,float);
	void PhysicsManager::stepPhysics();
};

