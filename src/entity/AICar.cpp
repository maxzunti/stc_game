#include "AICar.h"
#include <glm/gtx/quaternion.hpp>
#include <math.h>
#include <glm/gtx/vector_angle.hpp>
#include <time.h>

using namespace glm;

AICar::AICar(CarColor col, std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents, StaticPhysicsObject * track, std::vector<RectTrigger*> AInodes) :
	Car(col, model_fname, tex_fname, actor, physicsManager, ents, track, AInodes)
{
    srand(time(NULL));
    this->mActor->setName("AICar");
}

AICar::~AICar()
{
	for (int i = 0; i < NUM_WHEELS; i++) {
		delete wheels[i];
	}
	physMan->mScene->removeActor(*mVehicleNoDrive->getRigidDynamicActor());
}

//Fires the hook
void AICar::fireHook(glm::vec3 target) {
    min_hookDist = 999999.0f;

    //  this->myJB->playEffect(myJB->firehook);
    glm::quat q;
    glm::vec3 aim = target - this->getPos();
    aim = glm::normalize(aim);
    glm::vec3 upvec = glm::vec3(0, 1, 0);
    glm::vec3 a = glm::cross(upvec, aim);
    q.x = a.x;
    q.y = a.y;
    q.z = a.z;
    q.w = sqrt((glm::length(upvec)*glm::length(upvec) * (glm::length(aim)*glm::length(aim)))) + glm::dot(upvec, aim);
    q = glm::normalize(q);
        

    this->mPhysicsManager->mScene->addActor(*myHook->mActor);
    myHook->setShot(true);
    myHook->setRot(q);
    
    glm::vec3 b = this->getPos()+HOOK_START_DIST*this->getDir();
    myHook->setPos(b.x + (HOOK_START_DIST*aim.x), b.y + HOOK_START_DIST, b.z + (HOOK_START_DIST*aim.z));
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
	
    if (devChange)
    {
        DEVIATION = (rand() % 20) - 10.0f;
        devChange = false;
    }

	vec3 start = this->getPos();

	//getpos of destination node
	vec3 goal = nodes[this->partoflap]->getPos();
    vec3 goalDir = vec3(goal - start);
	//pathfind; find the direction vector from here to node pos, compare it with dir

    vec3 mydir = glm::normalize(this->dir);
    vec3 binorm = glm::normalize(glm::cross(goalDir, vec3(0, 1, 0)));
	goal += DEVIATION*binorm;
    goalDir = (goal - start);
	vec3 desDir = glm::normalize(goalDir);
	
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

    if (myHook->getStuck()) {

        PxVec3 hookDir = PxVec3(myHook->getPos().x, myHook->getPos().y, myHook->getPos().z) -
            PxVec3(pos.x, pos.y, pos.z);
        float hookDist = hookDir.normalize();

        if (hookDist <= min_hookDist + 0.3) {
            min_hookDist = hookDist;
        }
        else { // min_hookDist < curr hook
            if (!swinging) {
                cancelHook();
            }
        }
    }

	// Must fire after calc aim
    /*
	if ((!this->myHook->getShot() && !this->myHook->getStuck() && calcAim())) {
		//arrow->reposition(up, pos, aim, aim_rot);
		fireHook();
	}*/

	if (this->myHook->getStuck()) {
		this->retracting = true;
        //     this->myJB->playEffect(myJB->gravpull);
	}

	if (this->retracting)
	{
		this->retractHook();
	}

    //Defines the distance that the hook detaches at
    if (((getHookDistance() > HOOK_MAX_LENGTH) && (myHook->getShot())) || ((getHookDistance() < HOOK_MIN_LENGTH) && (myHook->getStuck()))) {
        cancelHook();
    }

	this->myHook->update(pos + (HOOK_FORWARD_OFFSET*dir) + (HOOK_UP_OFFSET*up));
}




