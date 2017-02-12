#include "PhysicsManager.h"

using namespace physx;

PhysicsManager::PhysicsManager(PxContactModifyCallback* callBack)
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
    sceneDesc.contactModifyCallback = callBack;
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

PxActor* PhysicsManager::createWallPlane(float xpos, float ypos, float zpos, float xnorm, float znorm)
{
	PxRigidStatic* wallPlane = PxCreatePlane(*mPhysics, PxPlane(PxVec3(xpos, ypos, zpos), PxVec3(xnorm, 0, znorm)), *mMaterial);
    PxShape* shapes[1];
    wallPlane->getShapes(shapes, 1);
    mScene->addActor(*wallPlane);


    PxFilterData filterData;
    filterData.word0 = COLLISION_FLAG_OBSTACLE;
    filterData.word1 = COLLISION_FLAG_OBSTACLE_AGAINST;

    shapes[0]->setSimulationFilterData(filterData);

	return wallPlane;

}

PxRigidBody* PhysicsManager::createBlock(float x, float y, float z, float xdim, float ydim, float zdim)
{
	//PxShape* shape = mPhysics->createShape(PxBoxGeometry(1.0f, 1.0f, 1.0f), *mMaterial);
	PxTransform localTm(PxVec3(x, y, z));
	PxRigidDynamic* body = mPhysics->createRigidDynamic(localTm);
	PxShape* shape = body->createShape(PxBoxGeometry(xdim, ydim, zdim), *mMaterial);
	//body->attachShape(*shape);
	body->setLinearDamping(1.0f);
	body->setAngularDamping(1.0f);
	PxRigidBodyExt::updateMassAndInertia(*body, 1.0f);
	//mScene->addActor(*body);
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