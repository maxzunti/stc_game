#pragma once
#include "PxPhysicsAPI.h"
#include "PxSimulationEventCallback.h"
#include "../entity/Car.h"
#include "../entity/AICar.h"

#include "../entity/RectTrigger.h"

class TriggerListener : public physx::PxSimulationEventCallback {
public:
    TriggerListener();
    ~TriggerListener();
    void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count);
    void onWake(physx::PxActor** actors, physx::PxU32 count);
    void onSleep(physx::PxActor** actors, physx::PxU32 count);
    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
};