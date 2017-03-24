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

class AICar : public Car {

protected:
	const float TURN_FACTOR = 1.f;
	const float ACCEL_FACTOR = 100.f;

	bool calcAim();
	float heuristic(glm::vec3 node);

public:
	AICar(CarColor col, std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents, StaticPhysicsObject * track, std::vector<RectTrigger*> AInodes);
	~AICar();

	void navigate();	
	virtual void update();
};