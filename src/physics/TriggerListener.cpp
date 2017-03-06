#include "TriggerListener.h"
#include <iostream>

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
    std::cout << "TRiggered" << std::endl;
    for (PxU32 i = 0; i < count; i++)
    {
        
        if ((pairs[i].otherShape->getActor()->getName() == "Car") &&
            (pairs[i].triggerShape->getActor()->getName() == "LapBox"))
        {
            Car* myCar = static_cast<Car*>(pairs[i].otherShape->getActor()->userData);
            RectTrigger* myTrig = static_cast<RectTrigger*>(pairs[i].triggerShape->getActor()->userData);
            if (myCar->partoflap == myTrig->triggerID)
            {
                std::cout << "Increment portion of lap!" << std::endl;
                myCar->partoflap++;
                if (myCar->partoflap == 3)
                {
                    std::cout << "Increment lap!" << std::endl;
                    myCar->lap++;
                }
            }
        }
    }
}

void TriggerListener::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) {
    std::cout << "TriggerListener::onContact called" << std::endl;
}