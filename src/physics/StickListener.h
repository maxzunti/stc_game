#pragma once
#include <iostream>
#include "PxPhysicsAPI.h"
#include "PxContactModifyCallback.h"

class StickListener : public PxContactModifyCallback
{
public:
	StickListener();
	~StickListener();
	void StickListener::onContactModify(PxContactModifyPair *const pairs, PxU32 count);
};



