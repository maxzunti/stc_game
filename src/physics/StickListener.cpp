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
            // Set velocity to ZERO!!!!!!
            for (PxU32 i = 0; i < pairs->contacts.size(); ++i) {
                pairs->contacts.setTargetVelocity(i, PxVec3(0, 0, -10));
            }
            //PxRigidBody *temp = pairs->actor[0];
            //pairs->contacts.ignore(// ->actor[0]->setActorFlags(PxActorFlag::eDISABLE_SIMULATION);
			std::cout << "first thing is hook" << std::endl;

		}
		else if (pairs->actor[1]->getName() == "Hook")
		{
			std::cout << "second thing is hook" << std::endl;
		}
	}
}