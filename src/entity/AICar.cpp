#include "AICar.h"
#include <glm/gtx/quaternion.hpp>
#include <math.h>
#include <glm/gtx/vector_angle.hpp>

using namespace glm;

AICar::AICar(CarColor col, std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents, StaticPhysicsObject * track, std::vector<RectTrigger*> AInodes) :
	Car(col, model_fname, tex_fname, actor, physicsManager, ents, track, AInodes)
{
	DEVIATION = rand() % 10;
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

    if (this->getPos().y < -200.0f)
    {
		//Respawn switches
        switch (this->partoflap)
        {
        case 0:
            this->setPos(-300, 10, -200);
            this->setRot(0.0, -0.5, 0.0);
            break;
        case 1:
            this->setPos(-150, 10, -350);
            this->setRot(0.0, -1.3, 0.0);
            break;
        case 2:
            this->setPos(80.1522, 70, 161.581);
            this->setRot(0.0, 1.57 / 2.0, 0.0);
            break;
        case 3:
            this->setPos(-988.594, 23, -207.866);
            this->setRot(0.0, 4.14 / 1.2, 0.0);
            break;
        default:
            std::cout << "switch error: ai car ded" << std::endl;
            break;
        }
    }
	
	vec3 start = this->getPos();

	//getpos of destination node
	vec3 goal = nodes[this->partoflap]->getPos();
	float x, y, z;

	//pathfind; find the direction vector from here to node pos, compare it with dir
	if (rand() % 1 > 0)
	{
		x = start.x - (goal.x + DEVIATION);
	}
	else
	{
		x = start.x - (goal.x - DEVIATION);
	}
	if (rand() % 1 > 0)
	{
		z = start.z - (goal.z + DEVIATION);
	}
	else
	{
		z = start.z - (goal.z - DEVIATION);
	}
	y = start.y - goal.y;
	vec3 desDir = -normalize(vec3(x, y, z));
    vec3 mydir = glm::normalize(this->dir);
	
	/*if (this->mActor->getLinearVelocity().magnitude() <= 1)
	{
		applyWheelTurn(TURN_FACTOR);
	}*/
    float turnangle = glm::angle(dir, desDir);
	if (dir.x*desDir.z-dir.z*desDir.x > 0)
	{
		//increase dir.x
		applyWheelTurn(turnangle*TURN_FACTOR);
	}
	else if (dir.x*desDir.z - dir.z*desDir.x < 0)
	{
		//decrease dir.x
		applyWheelTurn(-turnangle*TURN_FACTOR);
	}

	//Directly inputting forces
	//When should a car not be accelerating?

	resetBrakes();
	applyWheelTorque(ACCEL_FACTOR);
	

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
		//arrow->reposition(up, pos, aim, aim_rot);
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