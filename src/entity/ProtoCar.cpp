#include "ProtoCar.h"
#include <glm/gtx/quaternion.hpp>
#include <math.h>


using namespace glm;

 ProtoCar::ProtoCar(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, Input* cont, std::vector<Entity*> &ents) :
    PhysicsObject(model_fname, tex_fname, actor, physicsManager),
    arrow(new AimArrow("assets/models/AimArrow/AimArrow.obj", "assets/models/AimArrow/blue.png")),
	myHook(new Hook("assets/models/sphere/sphere.obj", "assets/models/sphere/blue.png", physicsManager->createHook(0.f, 100.0f, 0.0f, 0.25f, 0.25f, 0.25f), physicsManager, ents))
{
    myHook->scale(2.0, 2.0, 2.0);
    controller = cont;
    ents.push_back(arrow.get());
	ents.push_back(myHook.get());
    //Create a vehicle that will drive on the plane.
    VehicleDesc vehicleDesc = initVehicleDesc();
    mVehicleNoDrive = createVehicleNoDrive(vehicleDesc, physicsManager->mPhysics, physicsManager->mCooking);
    PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
    mVehicleNoDrive->getRigidDynamicActor()->setGlobalPose(startTransform);
   
    physicsManager->mScene->addActor(*mVehicleNoDrive->getRigidDynamicActor());
    this->mActor = mVehicleNoDrive->getRigidDynamicActor();
    this->retracting = false;

    initWheels("assets/models/Crate/Crate1.obj", "assets/models/Crate/CrateImage1.JPG");
    for (int i = 0; i < NUM_WHEELS; i++) {
        ents.push_back(wheels[i]);
    }
}

 ProtoCar::~ProtoCar() {
     for (int i = 0; i < NUM_WHEELS; i++) {
         delete wheels[i];
     }
 }

 // Setup the wheels for rendering
 void ProtoCar::initWheels(std::string model_fname, std::string tex_fname) {
     for (int i = 0; i < NUM_WHEELS; i++) {
         wheels[i] = new Renderable(model_fname, tex_fname);
     }
 }

void ProtoCar::applyGlobalForce(glm::vec3 direction, double magnitude) {
    PxVec3 physVec(direction.x, direction.y, direction.z);
    PxRigidBodyExt::addForceAtLocalPos(*mActor, physVec * magnitude, PxVec3(0, 0, 0));
}

void ProtoCar::applyLocalForce(float forward, float right, float up) {
    PxVec3 physVec(right, up, -forward); // currently crossing these over
    PxRigidBodyExt::addLocalForceAtLocalPos(*mActor, physVec, PxVec3(0, 0, 0));
    
}

void ProtoCar::calcAim() {
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

void ProtoCar::update() {

    //Apply turn according to the left stick angle 
    applyWheelTurn(controller->LStick_InDeadzone() ? 0.f : controller->LeftStick_X());
   
    // Use the triggers to accelerate(Right Trigger) or reverse (Left Trigger)
    if ((controller->RightTrigger() - controller->LeftTrigger()) > 0) {
        resetBrakes();
        applyWheelTorque((controller->RightTrigger() - controller->LeftTrigger()));
    } else if ((controller->LeftTrigger() - controller->RightTrigger()) > 0) {
        resetBrakes();
        applyWheelTorque(-1.f*(controller->LeftTrigger() - controller->RightTrigger()));
        
    } else {
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
  /*  
    Debug rotations - don't really need this anymore
    if (controller->GetButtonPressed(XButtonIDs::L_Shoulder)) {
        rotate(0., 0.05, 0.);
    }*/
  
    // Perform physX update
    updatePosandRot();

    // Update aim (after PhysX!)
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



/*
       ^^
(0)|  ----  |(1)
       ||
       ||
       ||
(2)|  ----  |(3)

*/
void ProtoCar::applyWheelTurn(float factor) {
    this->mVehicleNoDrive->setSteerAngle(0,-factor/(15.f*(this->mActor->getLinearVelocity().magnitude()/MAX_SPEED)+1.0f));
    this->mVehicleNoDrive->setSteerAngle(1,-factor/(15.f*(this->mActor->getLinearVelocity().magnitude()/MAX_SPEED)+1.0f));

  //  this->mVehicleNoDrive->mWheelsDynData.pose
}

void ProtoCar::applyWheelTorque(float factor) {
    this->mVehicleNoDrive->setDriveTorque(0, -FORCE_FACTOR*factor);
    this->mVehicleNoDrive->setDriveTorque(1, -FORCE_FACTOR*factor);
    this->mVehicleNoDrive->setDriveTorque(2, -FORCE_FACTOR*factor);
    this->mVehicleNoDrive->setDriveTorque(3, -FORCE_FACTOR*factor);
}


void ProtoCar::resetBrakes()
{
    this->mVehicleNoDrive->setBrakeTorque(0, 0);
    this->mVehicleNoDrive->setBrakeTorque(1, 0);
    this->mVehicleNoDrive->setBrakeTorque(2, 0);
    this->mVehicleNoDrive->setBrakeTorque(3, 0);
}

void ProtoCar::startBrakeMode()
{
    this->mVehicleNoDrive->setBrakeTorque(0, FORCE_FACTOR*100000.0f);
    this->mVehicleNoDrive->setBrakeTorque(1, FORCE_FACTOR*100000.0f);
    this->mVehicleNoDrive->setBrakeTorque(2, FORCE_FACTOR*100000.0f);
    this->mVehicleNoDrive->setBrakeTorque(3, FORCE_FACTOR*100000.0f);
}

glm::vec3 ProtoCar::getAim() const {
    return aim;
}

glm::quat ProtoCar::getAimRot() const {
    return aim_rot;
}

VehicleDesc ProtoCar::initVehicleDesc()
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.

    // TODO: Generalize these to be private class members (which we can modify at runtime with our config parser)

    const PxF32 chassisMass = 1500.0f; //10 - too low may cause vibration/jitter

    //TODO: must change the MOI of the chassis once we add multi-level tracks (ie ramps)
    const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
    const PxVec3 chassisMOI
        ((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
            (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
            (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
    const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f-1.5f, 0.25f); // COG -1

    const PxU32 nbWheels = 4;

    VehicleDesc vehicleDesc;
    vehicleDesc.chassisMass = chassisMass;
    vehicleDesc.chassisDims = chassisDims;
    vehicleDesc.chassisMOI = chassisMOI;
    vehicleDesc.chassisCMOffset = chassisCMOffset;
    vehicleDesc.chassisMaterial = this->mPhysicsManager->mMaterial;

    vehicleDesc.wheelMass = WHEEL_MASS;
    vehicleDesc.wheelRadius = WHEEL_RAD;
    vehicleDesc.wheelWidth = WHEEL_WIDTH;
    vehicleDesc.wheelMOI = WHEEL_MOI;

    vehicleDesc.numWheels = nbWheels;
    vehicleDesc.wheelMaterial = this->mPhysicsManager->mMaterial;
    return vehicleDesc;
}

// Update the rendered position & rotation for each wheel
void ProtoCar::updateWheels(PxWheelQueryResult wheelQueryResults[NUM_WHEELS]) {
    PxTransform carPose = mActor->getGlobalPose();
   
    // Essentially copying PhysicsObject::updatePosAndRot()
    for (int i = 0; i < NUM_WHEELS; i++) {
        PxTransform wheelPose = wheelQueryResults[i].localPose;

        // Rotate by car rot
        glm::vec3 newPos = glm::rotate(qrot, glm::vec3(wheelPose.p.x, wheelPose.p.y, wheelPose.p.z));

        wheels[i]->setPos(carPose.p.x + newPos.x,
                          carPose.p.y + newPos.y,
                          carPose.p.z + newPos.z);

        PxQuat wheelRot = carPose.q * wheelPose.q;
        wheels[i]->setRot(glm::quat(wheelRot.w, wheelRot.x, wheelRot.y, wheelRot.z));
    }
}

void ProtoCar::stepForPhysics() {
    //Raycasts.
    PxVehicleWheels* vehicles[1] = { this->mVehicleNoDrive };
    PxRaycastQueryResult* raycastResults = this->mPhysicsManager->mVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
    const PxU32 raycastResultsSize = this->mPhysicsManager->mVehicleSceneQueryData->getRaycastQueryResultBufferSize();
    PxVehicleSuspensionRaycasts(this->mPhysicsManager->mBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

    //Vehicle update.
    const PxVec3 grav = this->mPhysicsManager->mScene->getGravity();

    /* PX_MAX_NB_WHEELS is 20 - probably a way bigger buffer than we need
       also, no need to reallocate this for each fn call
    PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
    PxVehicleWheelQueryResult vehicleQueryResults[1] = { { wheelQueryResults, this->mVehicleNoDrive->mWheelsSimData.getNbWheels() } };
    */

    PxWheelQueryResult wheelQueryResults[NUM_WHEELS];
    PxVehicleWheelQueryResult vehicleQueryResults = { wheelQueryResults, this->mVehicleNoDrive->mWheelsSimData.getNbWheels() } ;
    PxVehicleUpdates(1/60.f, grav, *this->mPhysicsManager->mFrictionPairs, 1, vehicles, &vehicleQueryResults);

    // Updates the renderable positions for each wheel
    updateWheels(wheelQueryResults);
}

//Fires the hook
void ProtoCar::fireHook() {
    this->mPhysicsManager->mScene->addActor(*myHook->mActor);
    myHook->setShot(true);
    myHook->setRot(aim_rot);

    glm::vec3 b = arrow->getPos();
    myHook->setPos(b.x+(2.0f*aim.x), b.y + 2.0f, b.z+(2.0f*aim.z));
    //glm::vec3 a_pos = arrow->getPos();
    //myHook->setPos(a_pos.x, a_pos.y + 2.0f, a_pos.z);
}

void ProtoCar::cancelHook() {
    if (!this->myHook->getStuck())
        this->mPhysicsManager->mScene->removeActor(*myHook->mActor);
    myHook->setShot(false);
    myHook->setStuck(false);
    myHook->setPos(0.0, 200.0, 0.0);
    myHook->mActor->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));
    this->retracting = false;
}

void ProtoCar::retractHook() {

    PxVec3 launchDir = PxVec3(this->myHook->getPos().x, this->myHook->getPos().y, this->myHook->getPos().z) -
        PxVec3(pos.x, pos.y, pos.z);

    // Implement this again when cooldown is working
    /*if (launchDir.magnitude() < 30.f)
    {
    this->cancelHook();
    }*/

    launchDir.normalize();

    this->mActor->setLinearVelocity(this->mActor->getLinearVelocity() + launchDir*10.f);
    this->mActor->setAngularVelocity(PxVec3(0.f, 0.f, 0.f));
}