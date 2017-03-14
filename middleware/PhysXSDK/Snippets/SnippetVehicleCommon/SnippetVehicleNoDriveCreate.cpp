/*
 * Copyright (c) 2008-2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * NVIDIA CORPORATION and its licensors retain all intellectual property
 * and proprietary rights in and to this software, related documentation
 * and any modifications thereto.  Any use, reproduction, disclosure or
 * distribution of this software and related documentation without an express
 * license agreement from NVIDIA CORPORATION is strictly prohibited.
 */
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

#include "SnippetVehicleCreate.h"
#include "SnippetVehicleTireFriction.h"
#include "SnippetVehicleRaycast.h"

namespace NoDrive
{

void computeWheelCenterActorOffsets
(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 numWheels, PxVec3* wheelCentreOffsets, wheel_data* wheelData)
{
	//chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
	//The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
	//Compute a position for the front wheel and the rear wheel along the z-axis.
	//Compute the separation between each wheel along the z-axis.
	const PxF32 numLeftWheels = numWheels/2.0f;
	const PxF32 deltaZ = (wheelFrontZ - wheelRearZ)/(numLeftWheels-1.0f);
	//Set the outside of the left and right wheels to be flush with the chassis.
	//Set the top of the wheel to be just touching the underside of the chassis.
	for(PxU32 i = 0; i < numWheels; i+=2)
	{
		//Left wheel offset from origin.
		wheelCentreOffsets[i + 0] = PxVec3((-chassisDims.x + wheelWidth)*wheelData->WHEEL_X_FACTOR, -(chassisDims.y/2 + wheelRadius) - wheelData->WHEEL_Y_LOWER, wheelRearZ + i*deltaZ*wheelData->WHEEL_Z_FACTOR);
		//Right wheel offsets from origin.
		wheelCentreOffsets[i + 1] = PxVec3((+chassisDims.x - wheelWidth)*wheelData->WHEEL_X_FACTOR, -(chassisDims.y/2 + wheelRadius) - wheelData->WHEEL_Y_LOWER, wheelRearZ + i*deltaZ*wheelData->WHEEL_Z_FACTOR);
	}
}


void setupWheelsSimulationData
(const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
 const PxVec3& chassisCMOffset, const PxF32 chassisMass,
 PxVehicleWheelsSimData* wheelsSimData,
 wheel_data* wheelData,
 tire_data* tireData,
 susp_data* suspension)
{
	//Set up the wheels.
	PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
	{
		//Set up the wheel data structures with mass, moi, radius, width.
		for(PxU32 i = 0; i < numWheels; i++)
		{
			wheels[i].mMass = wheelData->WHEEL_MASS;
			wheels[i].mMOI = wheelData->WHEEL_MOI;
			wheels[i].mRadius = wheelData->WHEEL_RADIUS;
			wheels[i].mWidth = wheelData->WHEEL_WIDTH;
			wheels[i].mDampingRate = wheelData->DAMPING_RATE;
			wheels[i].mMaxBrakeTorque = wheelData->MAX_BRAKE_TORQUE;
			wheels[i].mMaxSteer = wheelData->MAX_STEER;
			wheels[i].mToeAngle = wheelData->TOE_ANGLE;
		}
	}

	//Set up the tires.
	PxVehicleTireData tires[PX_MAX_NB_WHEELS];
	{
		//Set up the tires.
		for(PxU32 i = 0; i < numWheels; i++)
		{
			tires[i].mType = TIRE_TYPE_NORMAL;
			tires[i].mLatStiffX = tireData->LAT_STIFF_X;
			tires[i].mLatStiffY = tireData->LAT_STIFF_Y;
			tires[i].mLongitudinalStiffnessPerUnitGravity = tireData->LONG_STIFF_PER_UNIT_GRAV;
			
			// Tire friction-slip graph data
			tires[i].mFrictionVsSlipGraph[0][1] = tireData->G_FRIC_AT_ZERO_LONG_SLIP;
			tires[i].mFrictionVsSlipGraph[1][0] = tireData->G_LONG_SLIP_W_MAX_FRICTION;
			tires[i].mFrictionVsSlipGraph[1][1] = tireData->G_MAX_FRICTION;
			tires[i].mFrictionVsSlipGraph[2][0] = tireData->G_END_POINT;
			tires[i].mFrictionVsSlipGraph[2][1] = tireData->G_FRIC_PAST_END;
		}
	}

	//Set up the suspensions
	PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
	{
		//Compute the mass supported by each suspension spring.
		PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
		PxVehicleComputeSprungMasses(numWheels, wheelCenterActorOffsets, chassisCMOffset, chassisMass, 1, suspSprungMasses);

		//Set the suspension data.
		for(PxU32 i = 0; i < numWheels; i++)
		{
			suspensions[i].mMaxCompression = suspension->S_MAX_COMPRESSION;//0.3
			suspensions[i].mMaxDroop = suspension->S_MAX_DROOP;//0.1
			suspensions[i].mSpringStrength = suspension->S_SPRING_STRENGTH; //3500000	
			suspensions[i].mSpringDamperRate = suspension->S_SPRING_DAMPER_RATE;//4500
			suspensions[i].mSprungMass = suspSprungMasses[i];
		}

		//Set the camber angles.
		const PxF32 camberAngleAtRest=0.0;
		const PxF32 camberAngleAtMaxDroop=0.01f;
		const PxF32 camberAngleAtMaxCompression=-0.01f;
		for(PxU32 i = 0; i < numWheels; i+=2)
		{
			suspensions[i + 0].mCamberAtRest =  camberAngleAtRest;
			suspensions[i + 1].mCamberAtRest =  -camberAngleAtRest;
			suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
			suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
			suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
			suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
		}
	}

	//Set up the wheel geometry.
	PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
	PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
	PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
	PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
	{
		//Set the geometry data.
		for(PxU32 i = 0; i < numWheels; i++)
		{
			//Vertical suspension travel.
			suspTravelDirections[i] = PxVec3(0,-1,0);

			//Wheel center offset is offset from rigid body center of mass.
			wheelCentreCMOffsets[i] = wheelCenterActorOffsets[i] - chassisCMOffset;

			//Suspension force application point 0.3 metres below rigid body center of mass.
			suspForceAppCMOffsets[i]=PxVec3(wheelCentreCMOffsets[i].x,suspension->S_SUSP_APPLY_OFFSET,wheelCentreCMOffsets[i].z);

			//Tire force application point 0.3 metres below rigid body center of mass.
			tireForceAppCMOffsets[i]=PxVec3(wheelCentreCMOffsets[i].x,suspension->S_WHEEL_APPLY_OFFSET,wheelCentreCMOffsets[i].z);
		}
	}

	//Set up the filter data of the raycast that will be issued by each suspension.
	PxFilterData qryFilterData;
	setupNonDrivableSurface(qryFilterData);

	//Set the wheel, tire and suspension data.
	//Set the geometry data.
	//Set the query filter data
	for(PxU32 i = 0; i < numWheels; i++)
	{
		wheelsSimData->setWheelData(i, wheels[i]);
		wheelsSimData->setTireData(i, tires[i]);
		wheelsSimData->setSuspensionData(i, suspensions[i]);
		wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
		wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
		wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
		wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
		wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
		wheelsSimData->setWheelShapeMapping(i, i); 
	}
}

}// namespace NoDrive

PxVehicleNoDrive* createVehicleNoDrive(const VehicleDesc& vehicleDesc,
									   PxPhysics* physics,
									   PxCooking* cooking,
									   wheel_data* wheelData,
									   tire_data* tireData,
									   susp_data* suspension)
{
	const PxVec3 chassisDims = vehicleDesc.chassisDims;
	const PxF32 wheelWidth = vehicleDesc.wheelWidth;
	const PxF32 wheelRadius = vehicleDesc.wheelRadius;
	const PxU32 numWheels = vehicleDesc.numWheels;

	//Construct a physx actor with shapes for the chassis and wheels.
	//Set the rigid body mass, moment of inertia, and center of mass offset.
	PxRigidDynamic* vehActor = NULL;
	{
		//Construct a convex mesh for a cylindrical wheel.
		PxConvexMesh* wheelMesh = createWheelMesh(wheelWidth, wheelRadius, *physics, *cooking);
		//Assume all wheels are identical for simplicity.
		PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
		PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

		//Set the meshes and materials for the driven wheels.
		for(PxU32 i = 0; i < numWheels; i++)
		{
			wheelConvexMeshes[i] = wheelMesh;
			wheelMaterials[i] = vehicleDesc.wheelMaterial;
		}

		//Chassis just has a single convex shape for simplicity.
		PxConvexMesh* chassisConvexMesh = createChassisMesh(chassisDims, *physics, *cooking);
		PxConvexMesh* chassisConvexMeshes[1] = {chassisConvexMesh};
		PxMaterial* chassisMaterials[1] = {vehicleDesc.chassisMaterial};

		//Rigid body data.
		PxVehicleChassisData rigidBodyData;
		rigidBodyData.mMOI = vehicleDesc.chassisMOI;
		rigidBodyData.mMass = vehicleDesc.chassisMass;
		rigidBodyData.mCMOffset = vehicleDesc.chassisCMOffset;

		vehActor = createVehicleActor
			(rigidBodyData,
			wheelMaterials, wheelConvexMeshes, numWheels,
			chassisMaterials, chassisConvexMeshes, 1,
			*physics);
	}

	//Set up the sim data for the wheels.
	PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(numWheels);
	{
		//Compute the wheel center offsets from the origin.
		PxVec3 wheelCentreActorOffsets[PX_MAX_NB_WHEELS];
		const PxF32 frontZ = chassisDims.z * wheelData->WHEEL_FRONT_Z;
		const PxF32 rearZ = -chassisDims.z * wheelData->WHEEL_BACK_Z;
		NoDrive::computeWheelCenterActorOffsets(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, numWheels, wheelCentreActorOffsets, wheelData);

		NoDrive::setupWheelsSimulationData
			(numWheels, wheelCentreActorOffsets,
			 vehicleDesc.chassisCMOffset, vehicleDesc.chassisMass,
			 wheelsSimData, wheelData, tireData, suspension);
	}

	//Create a vehicle from the wheels and drive sim data.
	PxVehicleNoDrive* vehDriveNoDrive = PxVehicleNoDrive::allocate(numWheels);
	vehDriveNoDrive->setup(physics, vehActor, *wheelsSimData);

	//Free the sim data because we don't need that any more.
	wheelsSimData->free();

	return vehDriveNoDrive;
}



