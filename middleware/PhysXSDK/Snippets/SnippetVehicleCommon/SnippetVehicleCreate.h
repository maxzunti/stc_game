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

#ifndef SNIPPET_VEHICLE_COMMON_H
#define SNIPPET_VEHICLE_COMMON_H

#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleDriveTank.h"
#include "vehicle/PxVehicleNoDrive.h"


using namespace physx;

// Struct for holding suspension-related data; passed to 'createVehicleNoDrive'
// Created for Grav Grapplers, not actually part of PhysX
struct susp_data {
    // Default PhysX values
    float S_MAX_COMPRESSION = 0.3;
    float S_MAX_DROOP = 0.1;
    float S_SPRING_STRENGTH = 3500000.f;
    float S_SPRING_DAMPER_RATE = 4500.f;
};

struct wheel_data {
	float WHEEL_RADIUS = 0.0f;
	float WHEEL_MASS = 20.0f;
	float WHEEL_MOI = 0.0f;
	float WHEEL_WIDTH = 0.0f;
	float DAMPING_RATE = 0.25;
	float MAX_BRAKE_TORQUE = 1500.0f;
	float MAX_STEER = 0.0f;
	float TOE_ANGLE = 0.0f;
};

struct tire_data {
	float LAT_STIFF_X = 2.0f;
	float LAT_STIFF_Y = 0.3125f * (180.0f / PxPi);
	float LONG_STIFF_PER_UNIT_GRAV = 1000.0f;
	
	// Friction-graph data
	float G_FRIC_AT_ZERO_LONG_SLIP = 1.0f;
	float G_LONG_SLIP_W_MAX_FRICTION = 0.1f;
	float G_MAX_FRICTION = 1.0f;
	float G_END_POINT = 1.0f;
	float G_FRIC_PAST_END = 1.0f;
};

////////////////////////////////////////////////

PxRigidStatic* createDrivablePlane(physx::PxMaterial* material, PxPhysics* physics);

////////////////////////////////////////////////

PxConvexMesh* createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking);

PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking);

////////////////////////////////////////////////

PxRigidDynamic* createVehicleActor
(const PxVehicleChassisData& chassisData,
 PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels, 
 PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, 
 PxPhysics& physics);

////////////////////////////////////////////////

struct VehicleDesc
{
	PxF32 chassisMass;
	PxVec3 chassisDims;
	PxVec3 chassisMOI;
	PxVec3 chassisCMOffset;
	PxMaterial* chassisMaterial;
	PxF32 wheelMass;
	PxF32 wheelWidth;
	PxF32 wheelRadius;
	PxF32 wheelMOI;
	PxMaterial* wheelMaterial;
	PxU32 numWheels;
};

PxVehicleDrive4W* createVehicle4W(const VehicleDesc& vehDesc, PxPhysics* physics, PxCooking* cooking);

PxVehicleDriveTank* createVehicleTank(const VehicleDesc& vehDesc, PxPhysics* physics, PxCooking* cooking);

PxVehicleNoDrive* createVehicleNoDrive(const VehicleDesc& vehDesc, PxPhysics* physics, PxCooking* cooking, wheel_data* wheelData, tire_data* tireData, susp_data* suspension);

void customizeVehicleToLengthScale(const PxReal lengthScale, PxRigidDynamic* rigidDynamic, PxVehicleWheelsSimData* wheelsSimData, PxVehicleDriveSimData* driveSimData);

void customizeVehicleToLengthScale(const PxReal lengthScale, PxRigidDynamic* rigidDynamic, PxVehicleWheelsSimData* wheelsSimData, PxVehicleDriveSimData4W* driveSimData);


////////////////////////////////////////////////

#endif //SNIPPET_VEHICLE_COMMON_H
