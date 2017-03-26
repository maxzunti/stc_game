#include "StickListener.h"

using namespace physx;

StickListener::StickListener()
{

}

StickListener::~StickListener()
{

}

void StickListener::onContactModify(PxContactModifyPair *const pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; i++)
	{
	//PxTransform pointOfContact;
        
		if (pairs[i].actor[0]->getName() == "Hook")
		{
            // Set velocity to zero if wall, or car's velocity if car.
            for (PxU32 j = 0; j < pairs[i].contacts.size(); ++j) {
                pairs[i].contacts.setTargetVelocity(j, PxVec3(0, 0, 0));
            }
            Hook* temp = static_cast<Hook*>(pairs[i].actor[0]->userData);
            temp->setStuck(true);
            std::cout << "Set Hook to Stuck" << std::endl;
            temp->mActor->setLinearVelocity(PxVec3(0, 0, 0));
            temp->mActor->setAngularVelocity(PxVec3(0, 0, 0));
            temp->mActor->setGlobalPose(temp->mActor->getGlobalPose());
        }

        else if (pairs[i].actor[1]->getName() == "Hook")
        {
            // Set velocity to zero if wall, or car's velocity if car.
            for (PxU32 j = 0; j < pairs[i].contacts.size(); ++j) {
                pairs[i].contacts.setTargetVelocity(j, PxVec3(0, 0, 0));
            }
            
            Hook* temp = static_cast<Hook*>(pairs[i].actor[1]->userData);
            temp->setStuck(true);
            std::cout << "Set Hook to Stuck" << std::endl;
            temp->mActor->setLinearVelocity(PxVec3(0, 0, 0));
            temp->mActor->setAngularVelocity(PxVec3(0, 0, 0));
            temp->mActor->setGlobalPose(temp->mActor->getGlobalPose());
        }
    }
}