#include "AICar.h"
#include <glm/gtx/quaternion.hpp>
#include <math.h>

using namespace glm;

//Plays with their own controller...
//Should we even use this?  Perhaps make an AIInput instead

//This doesn't make any sense.  ProtoCar makes 0 decisions, so you would need to give AICar a way to do so
//But then what does it output?  Directly to the PhsyX Car?
//Where does it get input?  If it does not recieve input from a "controller" class, it would need to generate its own

//The below functions would all need to be rewritten from their ProtoCar counterparts, at the least

void AICar::calcAim() {
	vec3 right = normalize(cross(dir, vec3(0, 1, 0)));
	up = normalize(cross(right, dir));
	float angle = 0.0f;
	if (!controller->RStick_InDeadzone()) {
		angle = atan(controller->RightStick_Y() /
			controller->RightStick_X()) - (M_PI / 2);
		if (controller->RightStick_X() < 0) { // Stick in left half
			angle -= M_PI;
		}
	}
	aim_rot = glm::rotate(qrot, angle, up);
	aim = glm::rotate(aim_rot, vec3(0, 0, -1));
}

void AICar::update() {
	if (controller->GetButtonPressed(XButtonIDs::X)) {
		car_parser.updateFromFile();
		make_physX_car();
	}

	if (controller->GetButtonPressed(XButtonIDs::A)) {
		setPos(-50, 10, 200);
		setRot(0.0, 1.57 / 2.0, 0.0);
	}

	//Apply turn according to the left stick angle 
	applyWheelTurn(controller->LStick_InDeadzone() ? 0.f : controller->LeftStick_X());

	// Use the triggers to accelerate(Right Trigger) or reverse (Left Trigger)
	if ((controller->RightTrigger() - controller->LeftTrigger()) > 0) {
		resetBrakes();
		applyWheelTorque((controller->RightTrigger() - controller->LeftTrigger()));
	}
	else if ((controller->LeftTrigger() - controller->RightTrigger()) > 0) {
		resetBrakes();
		applyWheelTorque(-1.f*(controller->LeftTrigger() - controller->RightTrigger()));
	}
	else {
		applyWheelTorque(0);
		startBrakeMode();
	}

	//Cap the max velocity of the car to 80
	if (this->mActor->getLinearVelocity().magnitude() > MAX_SPEED && !this->retracting)
	{
		PxVec3 temp = this->mActor->getLinearVelocity();
		temp.normalize();
		this->mActor->setLinearVelocity(MAX_SPEED*temp);
	}
	else if (this->mActor->getLinearVelocity().magnitude() > MAX_SPEED*1.5f && this->retracting)
	{
		PxVec3 temp = this->mActor->getLinearVelocity();
		temp.normalize();
		this->mActor->setLinearVelocity(MAX_SPEED*1.5f*temp);
	}

	//Handbrake - Possibly remove in future
	if (controller->GetButtonPressed(XButtonIDs::A)) {
		//applyLocalForce(0, 0, 2000);
		startBrakeMode();
	}
	if (this->myHook->getStuck() && controller->GetButtonPressed(XButtonIDs::B)) {
		this->cancelHook();
	}

	calcAim();
	arrow->reposition(up, pos, aim, aim_rot);

	// Must fire after calc aim
	if ((!this->myHook->getShot() && !this->myHook->getStuck()) && controller->GetButtonPressed(XButtonIDs::R_Shoulder)) {
		fireHook();
	}

	if (this->myHook->getStuck() && controller->GetButtonPressed(XButtonIDs::R_Shoulder)) {
		this->retracting = true;
	}

	if (this->retracting)
	{
		this->retractHook();
	}

	this->myHook->update(pos + (HOOK_FORWARD_OFFSET*dir) + (HOOK_UP_OFFSET*up));
}