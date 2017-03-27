#pragma once
#include <iostream>
#include "PxPhysicsAPI.h"
#include "PxContactModifyCallback.h"
#include "../entity/Hook.h"
#include "../entity/Car.h"

class StickListener : public physx::PxContactModifyCallback
{
public:
	StickListener();
	~StickListener();
	void StickListener::onContactModify(physx::PxContactModifyPair *const pairs, physx::PxU32 count);
};