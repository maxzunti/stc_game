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
	std::cout << "oncontactmodify called" << std::endl;
	for (PxU32 i = 0; i < count; i++)
	{
	//PxTransform pointOfContact;
		if (pairs->actor[0]->getName() == "Hook" || pairs->actor[1]->getName() == "Hook")
		{
            // Set velocity to zero if wall, or car's velocity if car.
            for (PxU32 i = 0; i < pairs->contacts.size(); ++i) {
                pairs->contacts.setTargetVelocity(i, PxVec3(0, 0, 0));
            }
            Hook* temp = static_cast<Hook*>(pairs->actor[0]->userData);
            temp->setStuck(true);

            temp->mActor->setLinearVelocity(PxVec3(0, 0, 0));
            temp->mActor->setAngularVelocity(PxVec3(0, 0, 0));

            temp->mActor->getScene()->removeActor(*temp->mActor);
		}
	}
}