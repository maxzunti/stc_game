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
#include <iostream>

using namespace physx;

PxFilterFlags VehicleFilterShader
(PxFilterObjectAttributes attributes0, PxFilterData filterData0, 
 PxFilterObjectAttributes attributes1, PxFilterData filterData1,
 PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
    PX_UNUSED(filterData0);
    PX_UNUSED(filterData1);
	PX_UNUSED(constantBlock);
	PX_UNUSED(constantBlockSize);

    // let triggers through
    if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eDEFAULT;
    }


	if (((filterData0.word0 & filterData1.word1)>0) && ((filterData1.word0 & filterData0.word1)>0))
	{
        std::cout << "COLLISION filterData0.word0: " << filterData0.word0 << " filterData1.word0: " << filterData1.word0 << std::endl;

		if (filterData0.word0 == COLLISION_FLAG_HOOK || filterData1.word0 == COLLISION_FLAG_HOOK)
		{
			//std::cout << "collision is hooked" << std::endl;
            // might need to change resolve to solve or simply add solve
            pairFlags = PxPairFlag::eMODIFY_CONTACTS | PxPairFlag::eRESOLVE_CONTACTS | PxPairFlag::eDETECT_CCD_CONTACT;
               // | PxPairFlag::eDETECT_DISCRETE_CONTACT;
			
			return PxFilterFlag::eDEFAULT;
		}
	}

    if ((0 == (filterData0.word0 & filterData1.word1)) && (0 == (filterData1.word0 & filterData0.word1)))
    {
        std::cout << "NO COLLISION filterData0.word0: " << filterData0.word0 << " filterData1.word0: " << filterData1.word0 << std::endl;
        return PxFilterFlag::eSUPPRESS;
    }

	pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eDETECT_CCD_CONTACT;

	return PxFilterFlags();
}
