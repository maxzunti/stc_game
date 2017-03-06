#include "AICar.h"
#include <glm/gtx/quaternion.hpp>
#include <math.h>

using namespace glm;

//Plays with their own controller...
//Should we even use this?  Perhaps make an AIInput instead

//This doesn't make any sense.  ProtoCar makes 0 decisions, so you would need to give AICar a way to do so
//But then what does it output?  Directly to the PhsyX Car?
//Where does it get input?  If it does not recieve input from a "controller" class, it would need to generate its own

//The below functions would all need to be rewritten from their Car counterparts, at the least

AICar::AICar(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents) :
	Car(model_fname, tex_fname, actor, physicsManager, ents)
{
}

AICar::~AICar()
{
	for (int i = 0; i < NUM_WHEELS; i++) {
		delete wheels[i];
	}
	physMan->mScene->removeActor(*mVehicleNoDrive->getRigidDynamicActor());
}

void AICar::navigate()
{
	//Not sure if we need this yet
}

float AICar::heuristic(vec3 node)
{
	float x = abs(this->getPos().x - node.x);
	float y = abs(this->getPos().y - node.y);
	float z = abs(this->getPos().z - node.z);

	float out = sqrt(x*x + y*y + z*z);
	return out;
}

bool AICar::calcAim() {
	vec3 right = normalize(cross(dir, vec3(0, 1, 0)));
	up = normalize(cross(right, dir));
	float angle = 0.0f;
	/*if (!controller->RStick_InDeadzone()) {
		angle = atan(controller->RightStick_Y() /
			controller->RightStick_X()) - (M_PI / 2);
		if (controller->RightStick_X() < 0) { // Stick in left half
			angle -= M_PI;
		}
	}*/
	bool lockOn = false;
	//Something to search for nearest target



	aim_rot = glm::rotate(qrot, angle, up);
	aim = glm::rotate(aim_rot, vec3(0, 0, -1));

	return lockOn;
}

void AICar::update()
{
	//TODO: Acquire an ordered (?) vector of all the nodes, outlining the path
	//std::vector<DynamicPhysicsObject*> nodes;
	
	vec3 start = this->getPos();

	//getpos of destination node
	//vec3 goal = nodes[tracker]->getPos();

	//Either something like this, or we can detect a collision/trigger zone
	/*if (start.x <= goal.x + DEV && start.x >= goal.x - DEV &&
		start.y <= goal.y + DEV && start.y >= goal.y - DEV &&
		start.z <= goal.z + DEV && start.z >= goal.z - DEV)
	{
		tracker++;
		if (tracker == nodes.size())
		{
			tracker = 0;
		}
		goal = nodes[tracker]->getPos();
	}*/

	//pathfind; find the direction vector from here to node pos, compare it with dir
	/*float x = start.x - goal.x;
	float y = start.y - goal.y;
	float z = start.z - goal.z;
	vec3 desDir = normalize(vec3(x, y, z));
	vec3 dir = this->getDir();
	
	if (this->mActor->getLinearVelocity().magnitude() <= 1)
	{
		applyWheelTurn(TURN_FACTOR);
	}

	if (dir.x > desDir.x)
	{
		//increase dir.x
		applyWheelTurn(desDir.x-dir.x);
	}
	else if (dir.x < desDir.x)
	{
		//decrease dir.x
		applyWheelTurn(dir.x - desDir.x);
	}
	if (dir.z > desDir.z)
	{
		//increase dir.z
		applyWheelTurn(desDir.z - dir.z);
	}
	else if (dir.z < desDir.z)
	{
		//decrease dir.z
		applyWheelTurn(dir.z - desDir.z);
	}*/

	//Apply turn according to the left stick angle 
	//applyWheelTurn(controller->LStick_InDeadzone() ? 0.f : controller->LeftStick_X());

	//Directly inputting forces
	//When should a car not be accelerating?
	if (this->mActor->getLinearVelocity().magnitude() != 0.1)
	{
		resetBrakes();
		applyWheelTorque(ACCEL_FACTOR);
	}
	//When does it brake?
	/*else if ((controller->LeftTrigger() - controller->RightTrigger()) > 0) {
		resetBrakes();
		applyWheelTorque(-1.f*(controller->LeftTrigger() - controller->RightTrigger()));
	}*/
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
	/*if (controller->GetButtonPressed(XButtonIDs::A)) {
		//applyLocalForce(0, 0, 2000);
		startBrakeMode();
	}
	if (this->myHook->getStuck() && controller->GetButtonPressed(XButtonIDs::B)) {
		this->cancelHook();
	}*/

	// Must fire after calc aim
	if ((!this->myHook->getShot() && !this->myHook->getStuck() && calcAim())) {
		arrow->reposition(up, pos, aim, aim_rot);
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