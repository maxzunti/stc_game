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

#include "../util/ConfigParser.h"

class AICar : public Car {
protected:
	void calcAim();
public:
	virtual void update();
};