#include "TriggerListener.h"
#include <iostream>
#include <time.h>

using namespace physx;

TriggerListener::TriggerListener() {

}

TriggerListener::~TriggerListener() {

}

void TriggerListener::onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {
    std::cout << "TriggerListener::onConstraintBreak called" << std::endl;
}
void TriggerListener::onWake(PxActor** actors, PxU32 count) {
    std::cout << "TriggerListener::onWake called" << std::endl;
}
void TriggerListener::onSleep(PxActor** actors, PxU32 count) {
    std::cout << "TriggerListener::onSleep called" << std::endl;
}
void TriggerListener::onTrigger(PxTriggerPair* pairs, PxU32 count) {
    
    for (PxU32 i = 0; i < count; i++)
    {

        if ((pairs[i].otherShape->getActor()->getName() == "AICar") &&
            (pairs[i].triggerShape->getActor()->getName() == "HookBox"))
        {
            AICar* myCar = static_cast<AICar*>(pairs[i].otherShape->getActor()->userData);
            RectTrigger* myTrig = static_cast<RectTrigger*>(pairs[i].triggerShape->getActor()->userData);

            if (!myCar->getHook()->getShot() && !myCar->getHook()->getStuck())
            {
                myCar->firehookbool = true;
                myCar->firehooktarget = myTrig->target;
            }
        }

        if ((pairs[i].otherShape->getActor()->getName() == "AICar") &&
            (pairs[i].triggerShape->getActor()->getName() == "FallBox"))
        {
            AICar* myCar = static_cast<AICar*>(pairs[i].otherShape->getActor()->userData);

                myCar->partoflap = 0;
        }


        if (((pairs[i].otherShape->getActor()->getName() == "Car") || (pairs[i].otherShape->getActor()->getName() == "AICar")) &&
            (pairs[i].triggerShape->getActor()->getName() == "LapBox"))
        {
            Car* myCar = static_cast<Car*>(pairs[i].otherShape->getActor()->userData);
            RectTrigger* myTrig = static_cast<RectTrigger*>(pairs[i].triggerShape->getActor()->userData);

            myCar->devChange = true;
            if (myCar->partoflap == myTrig->triggerID)
            {
                myCar->partoflap++;
                
            }
            if (myCar->partoflap == myCar->nodes.size() && myTrig->triggerID == 0)
            {
                if (myCar->lap == 3)
                {
                    myCar->partoflap = myCar->nodes.size()+1;
                }
                else
                {
                    myCar->lap++;
                    myCar->partoflap = 1;
                }
            }
        }
    }
}

void TriggerListener::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
    
}