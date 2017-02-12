#include "PhysicsManager.h"

using namespace physx;

PhysicsManager::PhysicsManager()
{
	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;
    



	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	
	bool recordMemoryAllocations = true;
	
	mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);

	if (!mProfileZoneManager)
		std::cout << "PxProfileZoneManager::createProfileZoneManager failed!" << std::endl;

	mTolerancesScale = PxTolerancesScale();

	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
		mTolerancesScale, recordMemoryAllocations, mProfileZoneManager);
	if (!mPhysics)
		std::cout << "PxCreatePhysics failed!" << std::endl;
    
    if (mPhysics->getPvdConnectionManager())
    {
        mPhysics->getVisualDebugger()->setVisualizeConstraints(true);
        mPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
        mPhysics->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
        gConnection = PxVisualDebuggerExt::createConnection(mPhysics->getPvdConnectionManager(), "127.0.0.1", 5425, 10);
    }
    
	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(mTolerancesScale));
	if (!mCooking)
		std::cout << "PxCreateCooking failed!" << std::endl;

    PxU32 numWorkers =2;

	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	mDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = mDispatcher;
	sceneDesc.filterShader = VehicleFilterShader;
    sceneDesc.contactModifyCallback = &mSticklisten;
	mScene = mPhysics->createScene(sceneDesc);

	mMaterial = mPhysics->createMaterial(.3f, .3f, 0.1f);

    /////////////////////////////////////////////
    
    PxInitVehicleSDK(*mPhysics);
    PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
    PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

    //Create the batched scene queries for the suspension raycasts.
    mVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, gDefaultAllocatorCallback);
    mBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *mVehicleSceneQueryData, mScene);

    //Create the friction table for each combination of tire and surface type.
    mFrictionPairs = createFrictionPairs(mMaterial);
	
	
	
}


PhysicsManager::~PhysicsManager()
{
    PxCloseVehicleSDK();
	mScene->release();
	mDispatcher->release();
	mPhysics->release();
	mProfileZoneManager->release();
	mFoundation->release();
}

PxActor* PhysicsManager::createGroundPlane()
{
	PxRigidStatic* groundPlane = PxCreatePlane(*mPhysics, PxPlane(PxVec3(0, 0, 0), PxVec3(0,1,0)), *mMaterial);
	mScene->addActor(*groundPlane);
	return groundPlane;
}

PxActor* PhysicsManager::createWallPlane(int x, int y, int z, int a, int b)
{
	PxRigidStatic* wallPlane = PxCreatePlane(*mPhysics, PxPlane(PxVec3(x, y, z), PxVec3(a, 0, b)), *mMaterial);
	mScene->addActor(*wallPlane);

	//TODO:Link this to COLLISION_FLAG_OBSTACLE
	setupFiltering(wallPlane, COLLISION_FLAG_OBSTACLE, COLLISION_FLAG_OBSTACLE_AGAINST);

	return wallPlane;
}

PxRigidBody* PhysicsManager::createBlock(float x, float y, float z)
{
	//PxShape* shape = mPhysics->createShape(PxBoxGeometry(1.0f, 1.0f, 1.0f), *mMaterial);
	PxTransform localTm(PxVec3(x, y, z));
	PxRigidDynamic* body = mPhysics->createRigidDynamic(localTm);
	PxShape* shape = body->createShape(PxBoxGeometry(1.0f, 1.0f, 1.0f), *mMaterial);
	//body->attachShape(*shape);
	body->setLinearDamping(1.0f);
	body->setAngularDamping(1.0f);
	PxRigidBodyExt::updateMassAndInertia(*body, 1.0f);
	mScene->addActor(*body);
	//////////////////////////
	PxFilterData filterData;
	filterData.word0 = COLLISION_FLAG_HOOK;
	filterData.word1 = COLLISION_FLAG_HOOK_AGAINST;

	shape->setSimulationFilterData(filterData);
	///////////////////////////
	return body;
}

void PhysicsManager::stepPhysics()
{
    PX_UNUSED(false);

	mScene->simulate(1/60.f);
	mScene->fetchResults(true);
}

void PhysicsManager::setupFiltering(PxRigidActor* actor, PxU32 group, PxU32 mask)
{
	
}