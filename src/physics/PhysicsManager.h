#pragma once
#include "PxPhysicsAPI.h"
#include <iostream>
#include <memory>
#include "vehicle/PxVehicleUtil.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleRaycast.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleTireFriction.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleFilterShader.h"
#include "../renderer/model/Model.h"

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

    PhysicsManager(PxSimulationEventCallback *event_callback, PxContactModifyCallback* mod_callback);
	~PhysicsManager();
	PxActor* PhysicsManager::createGroundPlane();
    PxActor* PhysicsManager::createWallPlane(float xpos, float ypos, float zpos, float xnorm, float znorm);
	PxRigidBody* PhysicsManager::createHook(float, float, float, float, float, float);
	PxRigidBody* PhysicsManager::createBlock(float x, float y, float z, float xdim, float ydim, float zdim);
    PxRigidStatic* PhysicsManager::createTriangleMesh(Model*, bool dynamic, PxU32 simFilterData, PxU32 simFilterDataAgainst, PxVec3 scale);
	void PhysicsManager::stepPhysics();
};

struct FilterGroup
{
	enum Enum
	{
		eCAR = (1 << 0),
		eHOOK = (1 << 1),
		eWALL = (1 << 2),
	};
};