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

#include <new>
#include "SnippetVehicleFilterShader.h"
#include "PxPhysicsAPI.h"
#include <PxFiltering.h>

using namespace physx;

PxFilterFlags VehicleFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
 PxFilterObjectAttributes attributes1, PxFilterData filterData1,
 PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

	if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
	{
		pairFlags = PxPairFlag::eMODIFY_CONTACTS;
		return PxFilterFlag::eDEFAULT;
	}

	if( (0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)) )
		return PxFilterFlag::eSUPPRESS;

	pairFlags = PxPairFlag::eCONTACT_DEFAULT;

	return PxFilterFlags();
}

/*PxFilterFlags PxSimulationFilterCallback::pairFound
(PxU32 pair,
	PxFilterObjectAttributes attributes0, PxFilterData d0,
	const PxActor * a0, const PxShape * s0,
	PxFilterObjectAttributes attributes1, PxFilterData d1,
	const PxActor * a1, const PxShape * s1,
	PxPairFlags & pairFlags)
{
	
}*/

class stick : public PxContactModifyCallback
{
	void onContactModify(PxContactModifyPair& pairs, PxU32 count)
	{
		for (PxU32 i = 0; i < count; i++)
		{
			//TODO: make them stick
			//pairs.actor[0]->getScene()->removeActor(pairs.actor[0]);
		}
	}
};
