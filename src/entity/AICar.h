#pragma once
#include "ProtoCar.h"
#include "PhysicsObject.h"

#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleUtil.h"
#include "vehicle/PxVehicleNoDrive.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleCreate.h"

#include "AimArrow.h"
#include "../input/input.h"
#include "Hook.h"

#include "../util/ConfigParser.h"

class AICar : public ProtoCar {
protected:
	void calcAim();
public:
	virtual void update();
};