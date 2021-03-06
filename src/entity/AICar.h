#pragma once
#include "Car.h"
#include "DynamicPhysicsObject.h"

#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleUtil.h"
#include "vehicle/PxVehicleNoDrive.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleCreate.h"

#include "AimArrow.h"
#include "../input/input.h"
#include "Hook.h"
#include "RectTrigger.h"
#include "../util/ConfigParser.h"
#include <vector>
#include <cstdlib>

class AICar : public Car {

protected:
	const float TURN_FACTOR = 2.5f;
	const float ACCEL_FACTOR = 1.f;
    const int AI_COOLDOWN = 3.0;
    bool swinging = false;
    
    const float reverseTime = 2.0f;
    std::clock_t reverseClock;


#include <ctime>

    float DEVIATION;


public:

	AICar(CarColor col, std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents, StaticPhysicsObject * track, std::vector<RectTrigger*> AInodes);
	virtual ~AICar();
	
	virtual void update();
    virtual bool isAI();
    void fireHook(glm::vec3 target);

    bool firehookbool = false;
    bool reverseMode = false;
    glm::vec3 firehooktarget;
};