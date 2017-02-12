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
	//TODO: make them stick
		if (pairs->actor[0]->getName() == "Hook")
		{
            // Set velocity to zero if wall, or car's velocity if car.
            for (PxU32 i = 0; i < pairs->contacts.size(); ++i) {
                pairs->contacts.setTargetVelocity(i, PxVec3(0, 0, 0));
            }
            static_cast<Hook*>(pairs->actor[0]->userData)->mStuck = true;

		}
		else if (pairs->actor[1]->getName() == "Hook")
		{
            // Set velocity to zero if wall, or car's velocity if car.
            for (PxU32 i = 0; i < pairs->contacts.size(); ++i) {
                pairs->contacts.setTargetVelocity(i, PxVec3(0, 0, 0));
            }
            static_cast<Hook*>(pairs->actor[1]->userData)->mStuck = true;
		}
	}
}