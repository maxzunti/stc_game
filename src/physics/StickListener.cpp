#include "StickListener.h"



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
			std::cout << "first thing is hook" << std::endl;

		}
		else if (pairs->actor[1]->getName() == "Hook")
		{
			std::cout << "second thing is hook" << std::endl;
		}
	}
}