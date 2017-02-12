#include "ProtoCar.h"
#include <glm/gtx/quaternion.hpp>
#include <math.h>


using namespace glm;

 ProtoCar::ProtoCar(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, Input* cont, std::vector<Entity*> &ents) :
    PhysicsObject(model_fname, tex_fname, actor, physicsManager),
    arrow(new AimArrow("assets/models/AimArrow/AimArrow.obj", "assets/models/AimArrow/blue.png")),
	myHook(new Hook("assets/models/Crate/Crate1.obj", "assets/models/teapot/teapot_tex.png", physicsManager->createBlock(0, 0, 0), physicsManager))
{
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
    
   // mVehicleNoDrive->setToRestState();
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
    applyWheelTurn(controller->LStick_InDeadzone() ? 0.f : controller->LeftStick_X());

    // Basic movement and rotation - this'll be heavily modified in the real car
  
    if ((controller->RightTrigger() - controller->LeftTrigger()) > 0) {
        this->mVehicleNoDrive->setBrakeTorque(0, 0);
        this->mVehicleNoDrive->setBrakeTorque(1, 0);
        this->mVehicleNoDrive->setBrakeTorque(2, 0);
        this->mVehicleNoDrive->setBrakeTorque(3, 0);
        applyWheelTorque((controller->RightTrigger() - controller->LeftTrigger()));
    } else {
        applyWheelTorque(0);
        this->mVehicleNoDrive->setBrakeTorque(0, FORCE_FACTOR * 100000);
        this->mVehicleNoDrive->setBrakeTorque(1, FORCE_FACTOR * 1000000);
        this->mVehicleNoDrive->setBrakeTorque(2, FORCE_FACTOR * 1000000);
        this->mVehicleNoDrive->setBrakeTorque(3, FORCE_FACTOR * 1000000);
    }
    if (this->mActor->getLinearVelocity().magnitude() > MAX_SPEED)
    {
        PxVec3 temp = this->mActor->getLinearVelocity();
        temp.normalize();
        this->mActor->setLinearVelocity(MAX_SPEED*temp);
    }
    if (pos.y <= 5.01 && controller->GetButtonPressed(XButtonIDs::A)) {
        applyLocalForce(0, 0, 2000);
    }
    if (controller->GetButtonPressed(XButtonIDs::L_Shoulder)) {
        rotate(0., 0.05, 0.);
    }
    if (controller->GetButtonPressed(XButtonIDs::R_Shoulder)) {
        //rotate(0., -0.05, 0.);
		//fireHook();
    }


    
    // Perform physX update
    updatePosandRot();

    // Update aim (after PhysX!)
    calcAim();
    arrow->reposition(up, pos, aim, aim_rot);
}

void ProtoCar::applyWheelTurn(float factor) {
    this->mVehicleNoDrive->setSteerAngle(2,factor/20.f);
    this->mVehicleNoDrive->setSteerAngle(3,factor/20.f);
}

void ProtoCar::applyWheelTorque(float factor) {


    this->mVehicleNoDrive->setDriveTorque(0, -FORCE_FACTOR*factor);
    this->mVehicleNoDrive->setDriveTorque(1, -FORCE_FACTOR*factor);
    this->mVehicleNoDrive->setDriveTorque(2, -FORCE_FACTOR*factor);
    this->mVehicleNoDrive->setDriveTorque(3, -FORCE_FACTOR*factor);
}

glm::vec3 ProtoCar::getAim() const {
    return aim;
}

VehicleDesc ProtoCar::initVehicleDesc()
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.

    const PxF32 chassisMass = 1500.0f; //10 - too low may cause vibration/jitter

    const PxVec3 chassisDims(2.5f, 2.0f, 5.0f);
    const PxVec3 chassisMOI
        (10000.f*(chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
            (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
            10000.f*(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
    const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f-0.0f, 0.25f);

    //Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    //Moment of inertia is just the moment of inertia of a cylinder.
    const PxF32 wheelMass = 20.f;//100
    const PxF32 wheelRadius = 0.5f;
    const PxF32 wheelWidth = 0.4f;
    const PxF32 wheelMOI = 0.1f*wheelRadius*wheelRadius;//0.1f*wheelRadius*wheelRadius;

    const PxU32 nbWheels = 4;

    VehicleDesc vehicleDesc;
    vehicleDesc.chassisMass = chassisMass;
    vehicleDesc.chassisDims = chassisDims;
    vehicleDesc.chassisMOI = chassisMOI;
    vehicleDesc.chassisCMOffset = chassisCMOffset;
    vehicleDesc.chassisMaterial = this->mPhysicsManager->mMaterial;
    vehicleDesc.wheelMass = wheelMass;
    vehicleDesc.wheelRadius = wheelRadius;
    vehicleDesc.wheelWidth = wheelWidth;
    vehicleDesc.wheelMOI = wheelMOI;
    vehicleDesc.numWheels = nbWheels;
    vehicleDesc.wheelMaterial = this->mPhysicsManager->mMaterial;
    return vehicleDesc;
}

void ProtoCar::startAccelerateForwardsMode()
{
    this->mVehicleNoDrive->setDriveTorque(0, -100000.0f);
    this->mVehicleNoDrive->setDriveTorque(1, -100000.0f);

}

void ProtoCar::startAccelerateReverseMode()
{
    this->mVehicleNoDrive->setDriveTorque(0, -1000.0f);
    this->mVehicleNoDrive->setDriveTorque(1, -1000.0f);
}

void ProtoCar::startBrakeMode()
{
    this->mVehicleNoDrive->setBrakeTorque(0, 1000.0f);
    this->mVehicleNoDrive->setBrakeTorque(1, 1000.0f);
    this->mVehicleNoDrive->setBrakeTorque(2, 1000.0f);
    this->mVehicleNoDrive->setBrakeTorque(3, 1000.0f);
}

void ProtoCar::startTurnHardLeftMode()
{
    this->mVehicleNoDrive->setDriveTorque(0, 1000.0f);
    this->mVehicleNoDrive->setDriveTorque(1, 1000.0f);
    this->mVehicleNoDrive->setSteerAngle(0, 1.0f);
    this->mVehicleNoDrive->setSteerAngle(1, 1.0f);
}

void ProtoCar::startTurnHardRightMode()
{
    this->mVehicleNoDrive->setDriveTorque(0, 1000.0f);
    this->mVehicleNoDrive->setDriveTorque(1, 1000.0f);
    this->mVehicleNoDrive->setSteerAngle(0, -1.0f);
    this->mVehicleNoDrive->setSteerAngle(1, -1.0f);
}

void ProtoCar::startHandbrakeTurnLeftMode()
{
    this->mVehicleNoDrive->setBrakeTorque(2, 1000.0f);
    this->mVehicleNoDrive->setBrakeTorque(3, 1000.0f);
    this->mVehicleNoDrive->setDriveTorque(0, 1000.0f);
    this->mVehicleNoDrive->setDriveTorque(1, 1000.0f);
    this->mVehicleNoDrive->setSteerAngle(0, 1.0f);
    this->mVehicleNoDrive->setSteerAngle(1, 1.0f);
}

void ProtoCar::startHandbrakeTurnRightMode()
{
    this->mVehicleNoDrive->setBrakeTorque(2, 1000.0f);
    this->mVehicleNoDrive->setBrakeTorque(3, 1000.0f);
    this->mVehicleNoDrive->setDriveTorque(0, 1000.0f);
    this->mVehicleNoDrive->setDriveTorque(1, 1000.0f);
    this->mVehicleNoDrive->setSteerAngle(0, -1.0f);
    this->mVehicleNoDrive->setSteerAngle(1, -1.0f);
}

void ProtoCar::releaseAllControls()
{
    this->mVehicleNoDrive->setDriveTorque(0, 0.0f);
    this->mVehicleNoDrive->setDriveTorque(1, 0.0f);
    this->mVehicleNoDrive->setDriveTorque(2, 0.0f);
    this->mVehicleNoDrive->setDriveTorque(3, 0.0f);

    this->mVehicleNoDrive->setBrakeTorque(0, 0.0f);
    this->mVehicleNoDrive->setBrakeTorque(1, 0.0f);
    this->mVehicleNoDrive->setBrakeTorque(2, 0.0f);
    this->mVehicleNoDrive->setBrakeTorque(3, 0.0f);

    this->mVehicleNoDrive->setSteerAngle(0, 0.0f);
    this->mVehicleNoDrive->setSteerAngle(1, 0.0f);
    this->mVehicleNoDrive->setSteerAngle(2, 0.0f);
    this->mVehicleNoDrive->setSteerAngle(3, 0.0f);
}

void ProtoCar::stepForPhysics() {
    //Raycasts.
    PxVehicleWheels* vehicles[1] = { this->mVehicleNoDrive };
    PxRaycastQueryResult* raycastResults = this->mPhysicsManager->mVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
    const PxU32 raycastResultsSize = this->mPhysicsManager->mVehicleSceneQueryData->getRaycastQueryResultBufferSize();
    PxVehicleSuspensionRaycasts(this->mPhysicsManager->mBatchQuery, 1, vehicles, raycastResultsSize, raycastResults);

    //Vehicle update.
    const PxVec3 grav = this->mPhysicsManager->mScene->getGravity();
    PxWheelQueryResult wheelQueryResults[PX_MAX_NB_WHEELS];
    PxVehicleWheelQueryResult vehicleQueryResults[1] = { { wheelQueryResults, this->mVehicleNoDrive->mWheelsSimData.getNbWheels() } };
    PxVehicleUpdates(1/60.f, grav, *this->mPhysicsManager->mFrictionPairs, 1, vehicles, vehicleQueryResults);
}

void ProtoCar::fireHook() {
	//Fires the hook
	glm::vec3 a = this->getAim();
	glm::vec3 b = arrow->getPos();
	myHook->setPos(b.x, b.y, b.z);
	//ents.push_back(myHook.get());
	myHook->applyGlobalForce(a, 50);
}