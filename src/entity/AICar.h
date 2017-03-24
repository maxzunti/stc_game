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
	const float TURN_FACTOR = 2.0f;
	const float ACCEL_FACTOR = 1.f;
    bool swinging = false;

    float DEVIATION;


public:
	AICar(CarColor col, std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents, StaticPhysicsObject * track, std::vector<RectTrigger*> AInodes);
	~AICar();
	
	virtual void update();
    void fireHook(glm::vec3 target);
};