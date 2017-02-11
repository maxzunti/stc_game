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
	mScene = mPhysics->createScene(sceneDesc);

	mMaterial = mPhysics->createMaterial(.5f, .5f, 0.1f);

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
	return wallPlane;
}

PxRigidBody* PhysicsManager::createBlock(float x, float y, float z)
{
	PxShape* shape = mPhysics->createShape(PxBoxGeometry(1.0f, 1.0f, 1.0f), *mMaterial);
	PxTransform localTm(PxVec3(x, y, z));
	PxRigidDynamic* body = mPhysics->createRigidDynamic(localTm);
	body->attachShape(*shape);
	body->setLinearDamping(1.0f);
	body->setAngularDamping(1.0f);
	PxRigidBodyExt::updateMassAndInertia(*body, 1.0f);
	mScene->addActor(*body);
	shape->release();
	return body;
}

void PhysicsManager::stepPhysics()
{
    PX_UNUSED(false);

	mScene->simulate(1.0f / 60.0f);
	mScene->fetchResults(true);
}

PxFilterFlags filterShader(
	PxFilterObjectAttributes a0, PxFilterData d0,
	PxFilterObjectAttributes a1, PxFilterData d1,
	PxPairFlags& pairFlags, const void* constantBlock, PxU32 cosntantBlockSize)
{
	if (PxFilterObjectIsTrigger(a0) || PxFilterObjectIsTrigger(a1))
	{
		pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
		return PxFilterFlag::eDEFAULT;
	}
	pairFlags = PxPairFlag::eCONTACT_DEFAULT;
	if ((d0.word0 & d1.word1) && (d1.word0 & d0.word1))
	{
		pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
	}
	return PxFilterFlag::eDEFAULT;
}

void setupFiltering(PxRigidActor* actor, PxU32 group, PxU32 mask)
{
	PxFilterData filterData;
	filterData.word0 = group;
	filterData.word1 = mask;

	const PxU32 num = actor->getNbShapes();
	PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*)*num);
	actor->getShapes(shapes, num);
	for (PxU32 i = 0; i < num; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	free(shapes);
}