#include "Car.h"
#include <glm/gtx/quaternion.hpp>
#include <math.h>

using namespace glm;

Car::Car(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, Input* cont, std::vector<Entity*> &ents) :
    DynamicPhysicsObject(model_fname, tex_fname, actor, physicsManager),
    arrow(new AimArrow("assets/models/AimArrow/AimArrow.obj", "assets/models/AimArrow/blue.png")),
    myHook(new Hook("assets/models/sphere/sphere.obj", "assets/models/sphere/blue.png", physicsManager->createHook(0.f, 100.0f, 0.0f, 0.25f, 0.25f, 0.25f), physicsManager, ents)),
    car_parser("config/car_config", &carParams)
{
    physMan = physicsManager;

    initParams();
    car_parser.updateFromFile();

    myHook->scale(2.0, 2.0, 2.0);
    controller = cont;
    ents.push_back(arrow.get());
    ents.push_back(myHook.get());

    initWheels("assets/models/wheel/wheel.obj", "assets/models/wheel/wheeltex.png");
    for (int i = 0; i < NUM_WHEELS; i++) {
        ents.push_back(wheels[i]);
    }

    make_physX_car();

    reset_scale();
    X_MODEL_SCALE = .7;
    Y_MODEL_SCALE = 1.;
    Z_MODEL_SCALE = 1.;
    scaleModels();
 //   scale(0.7f, 1.0f, 1.0f);
}

Car::Car(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents) :
	DynamicPhysicsObject(model_fname, tex_fname, actor, physicsManager),
	arrow(new AimArrow("assets/models/AimArrow/AimArrow.obj", "assets/models/AimArrow/blue.png")),
	myHook(new Hook("assets/models/sphere/sphere.obj", "assets/models/sphere/blue.png", physicsManager->createHook(0.f, 100.0f, 0.0f, 0.25f, 0.25f, 0.25f), physicsManager, ents)),
	car_parser("config/car_config", &carParams)
{
	physMan = physicsManager;

	initParams();
	car_parser.updateFromFile();

	myHook->scale(2.0, 2.0, 2.0);
	ents.push_back(arrow.get());
	ents.push_back(myHook.get());

	initWheels("assets/models/wheel/wheel.obj", "assets/models/wheel/wheeltex.png");
	for (int i = 0; i < NUM_WHEELS; i++) {
		ents.push_back(wheels[i]);
	}

	make_physX_car();

	reset_scale();
	X_MODEL_SCALE = .7;
	Y_MODEL_SCALE = 1.;
	Z_MODEL_SCALE = 1.;
	scaleModels();
//	scale(0.7f, 1.0f, 1.0f);
}

Car::~Car() {
    for (int i = 0; i < NUM_WHEELS; i++) {
        delete wheels[i];
    }
    physMan->mScene->removeActor(*mVehicleNoDrive->getRigidDynamicActor());
}

void Car::initParams() {
    carParams.push_back(std::make_pair(std::string("CHASSIS_MASS"), &CHASSIS_MASS));
    carParams.push_back(std::make_pair(std::string("CHASSIS_X"), &CHASSIS_X));
    carParams.push_back(std::make_pair(std::string("CHASSIS_Y"), &CHASSIS_Y));
    carParams.push_back(std::make_pair(std::string("CHASSIS_Z"), &CHASSIS_Z));
    carParams.push_back(std::make_pair(std::string("WHEEL_MASS"), &WHEEL_MASS));
    carParams.push_back(std::make_pair(std::string("WHEEL_RAD"), &WHEEL_RAD));
    carParams.push_back(std::make_pair(std::string("WHEEL_WIDTH"), &WHEEL_WIDTH));
    carParams.push_back(std::make_pair(std::string("WHEEL_MOI"), &WHEEL_MOI));
    carParams.push_back(std::make_pair(std::string("STEER_VEL_FACTOR"), &STEER_VEL_FACTOR));
    carParams.push_back(std::make_pair(std::string("BASE_STEER"), &BASE_STEER));
    carParams.push_back(std::make_pair(std::string("DRIVE_TORQUE"), &DRIVE_TORQUE));
    carParams.push_back(std::make_pair(std::string("BRAKE_TORQUE"), &BRAKE_TORQUE));
    carParams.push_back(std::make_pair(std::string("MAX_SPEED"), &MAX_SPEED));
    carParams.push_back(std::make_pair(std::string("TIRE_FRICTION"), &TIRE_FRICTION));
    carParams.push_back(std::make_pair(std::string("MAT_STATIC"), &MAT_STATIC));
    carParams.push_back(std::make_pair(std::string("MAT_DYNAMIC"), &MAT_DYNAMIC));
    carParams.push_back(std::make_pair(std::string("MAT_CR"), &MAT_CR));
    carParams.push_back(std::make_pair(std::string("CM_X"), &CM_X));
    carParams.push_back(std::make_pair(std::string("CM_Y"), &CM_Y));
    carParams.push_back(std::make_pair(std::string("CM_Z"), &CM_Z));
    carParams.push_back(std::make_pair(std::string("MAX_COMPRESSION"), &MAX_COMPRESSION));
    carParams.push_back(std::make_pair(std::string("MAX_DROOP"), &MAX_DROOP));
    carParams.push_back(std::make_pair(std::string("SPRING_STRENGTH"), &SPRING_STRENGTH));
    carParams.push_back(std::make_pair(std::string("SPRING_DAMPER_RATE"), &SPRING_DAMPER_RATE));
    carParams.push_back(std::make_pair(std::string("WHEEL_DAMPING_RATE"), &WHEEL_DAMPING_RATE));
    carParams.push_back(std::make_pair(std::string("WHEEL_MAX_BRAKE_TORQUE"), &WHEEL_MAX_BRAKE_TORQUE));
    carParams.push_back(std::make_pair(std::string("WHEEL_MAX_STEER"), &WHEEL_MAX_STEER));
    carParams.push_back(std::make_pair(std::string("WHEEL_TOE_ANGLE"), &WHEEL_TOE_ANGLE));
    carParams.push_back(std::make_pair(std::string("LAT_STIFF_X"), &LAT_STIFF_X));
    carParams.push_back(std::make_pair(std::string("LAT_STIFF_Y"), &LAT_STIFF_Y));
    carParams.push_back(std::make_pair(std::string("MAX_COMPRESSION"), &MAX_COMPRESSION));
    carParams.push_back(std::make_pair(std::string("LONG_STIFF_PER_UNIT_GRAV"), &LONG_STIFF_PER_UNIT_GRAV));
    carParams.push_back(std::make_pair(std::string("G_FRIC_AT_ZERO_LONG_SLIP"), &G_FRIC_AT_ZERO_LONG_SLIP));
    carParams.push_back(std::make_pair(std::string("G_LONG_SLIP_W_MAX_FRICTION"), &G_LONG_SLIP_W_MAX_FRICTION));
    carParams.push_back(std::make_pair(std::string("G_MAX_FRICTION"), &G_MAX_FRICTION));
    carParams.push_back(std::make_pair(std::string("G_END_POINT"), &G_END_POINT));
    carParams.push_back(std::make_pair(std::string("G_FRIC_PAST_END"), &G_FRIC_PAST_END));
    carParams.push_back(std::make_pair(std::string("CHASSIS_X_MOI_FACTOR"), &CHASSIS_X_MOI_FACTOR));
    carParams.push_back(std::make_pair(std::string("CHASSIS_Y_MOI_FACTOR"), &CHASSIS_Y_MOI_FACTOR));
    carParams.push_back(std::make_pair(std::string("CHASSIS_Z_MOI_FACTOR"), &CHASSIS_Z_MOI_FACTOR));
    carParams.push_back(std::make_pair(std::string("WHEEL_FRONT_Z"), &WHEEL_FRONT_Z));
    carParams.push_back(std::make_pair(std::string("WHEEL_BACK_Z"), &WHEEL_BACK_Z));
    carParams.push_back(std::make_pair(std::string("WHEEL_Y_LOWER"), &WHEEL_Y_LOWER));
    carParams.push_back(std::make_pair(std::string("WHEEL_X_FACTOR"), &WHEEL_X_FACTOR));
    carParams.push_back(std::make_pair(std::string("WHEEL_Z_FACTOR"), &WHEEL_Z_FACTOR));
}

//Create a vehicle that will drive on the plane.
void Car::make_physX_car() {
    if (mVehicleNoDrive) {
        physMan->mScene->removeActor(*mVehicleNoDrive->getRigidDynamicActor()); // delete the old car
    }

    for (int i = 0; i < NUM_WHEELS; i++) {
        wheels[i]->reset_scale();
        wheels[i]->SIL_X_SCALE = 1.2;
        wheels[i]->SIL_Y_SCALE = 1.2;
        wheels[i]->SIL_Z_SCALE = 1.2;
        wheels[i]->scaleModels();
        wheels[i]->scale(WHEEL_MODEL_SCL.x, WHEEL_MODEL_SCL.y, WHEEL_MODEL_SCL.z);
        wheels[i]->scale(WHEEL_RAD, WHEEL_WIDTH, WHEEL_RAD);
    }

    // Calculate those config-specified parameters which have associated formulas
    CM_Y = -CHASSIS_Y * 0.5f + CM_Y;
    WHEEL_MOI *= WHEEL_MASS * WHEEL_RAD * WHEEL_RAD; // ((factor) * mass) * wheelRadius * wheelRadius;
    LAT_STIFF_Y *= LAT_STIFF_Y * (180.0f / PxPi);


    wheel_data wheels;
    wheels.WHEEL_MASS = WHEEL_MASS;
    wheels.WHEEL_MOI = WHEEL_MOI;
    wheels.WHEEL_RADIUS = WHEEL_RAD;
    wheels.WHEEL_WIDTH = WHEEL_WIDTH;
    wheels.DAMPING_RATE = WHEEL_DAMPING_RATE;
    wheels.MAX_BRAKE_TORQUE = WHEEL_MAX_BRAKE_TORQUE;
    wheels.MAX_STEER = WHEEL_MAX_STEER;
    wheels.TOE_ANGLE = WHEEL_TOE_ANGLE;

    wheels.WHEEL_FRONT_Z = WHEEL_FRONT_Z;
    wheels.WHEEL_BACK_Z = WHEEL_BACK_Z;
    wheels.WHEEL_Y_LOWER = WHEEL_Y_LOWER;
    wheels.WHEEL_X_FACTOR = WHEEL_X_FACTOR;
    wheels.WHEEL_Z_FACTOR = WHEEL_Z_FACTOR;

    tire_data tires;
    tires.LAT_STIFF_X = LAT_STIFF_X;
    tires.LAT_STIFF_Y = LAT_STIFF_Y;
    tires.LONG_STIFF_PER_UNIT_GRAV = LONG_STIFF_PER_UNIT_GRAV;
    tires.G_FRIC_AT_ZERO_LONG_SLIP = G_FRIC_AT_ZERO_LONG_SLIP;
    tires.G_LONG_SLIP_W_MAX_FRICTION = G_LONG_SLIP_W_MAX_FRICTION;
    tires.G_MAX_FRICTION = G_MAX_FRICTION;
    tires.G_END_POINT = G_END_POINT;
    tires.G_FRIC_PAST_END = G_FRIC_PAST_END;

    susp_data suspension;
    suspension.S_MAX_COMPRESSION = MAX_COMPRESSION;
    suspension.S_MAX_DROOP = MAX_DROOP;
    suspension.S_SPRING_DAMPER_RATE = SPRING_DAMPER_RATE;
    suspension.S_SPRING_STRENGTH = SPRING_STRENGTH;

    VehicleDesc vehicleDesc = initVehicleDesc();

    mVehicleNoDrive = createVehicleNoDrive(vehicleDesc, physMan->mPhysics, physMan->mCooking, &wheels, &tires, &suspension);
    PxTransform startTransform(PxVec3(0, (vehicleDesc.chassisDims.y*0.5f + vehicleDesc.wheelRadius + 1.0f), 0), PxQuat(PxIdentity));
    mVehicleNoDrive->getRigidDynamicActor()->setGlobalPose(startTransform);

    physMan->mScene->addActor(*mVehicleNoDrive->getRigidDynamicActor());
    this->mActor = mVehicleNoDrive->getRigidDynamicActor();
    this->retracting = false;

    setPos(pos.x, pos.y + (2 * WHEEL_RAD) + (CHASSIS_Y)+1.0, pos.z);
    setRot(glm::quat());
}

// Setup the wheels for rendering
void Car::initWheels(std::string model_fname, std::string tex_fname) {
    for (int i = 0; i < NUM_WHEELS; i++) {
        wheels[i] = new Renderable(model_fname, tex_fname);
        wheels[i]->scale(WHEEL_MODEL_SCL.x, WHEEL_MODEL_SCL.y, WHEEL_MODEL_SCL.z);
    }
}

void Car::applyGlobalForce(glm::vec3 direction, double magnitude) {
    PxVec3 physVec(direction.x, direction.y, direction.z);
    PxRigidBodyExt::addForceAtLocalPos(*mActor, physVec * magnitude, PxVec3(0, 0, 0));
}

void Car::applyLocalForce(float forward, float right, float up) {
    PxVec3 physVec(right, up, -forward); // currently crossing these over
    PxRigidBodyExt::addLocalForceAtLocalPos(*mActor, physVec, PxVec3(0, 0, 0));

}

void Car::calcAim() {
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

void Car::update() {
    if (controller->GetButtonPressed(XButtonIDs::X)) {
        car_parser.updateFromFile();
        make_physX_car();
    }

    if (controller->GetButtonPressed(XButtonIDs::A)) {
        setPos(-50, 100, 72);
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
    /*
    Debug rotations - don't really need this anymore
    if (controller->GetButtonPressed(XButtonIDs::L_Shoulder)) {
    rotate(0., 0.05, 0.);
    }*/

    // Perform physX update
    //   updatePosandRot();

    // Update aim (after PhysX!)
    calcAim();
    arrow->reposition(up, pos, aim, aim_rot);

    // Must fire after calc aim
    if ((!this->myHook->getShot() && !this->myHook->getStuck()) && (controller->GetButtonPressed(XButtonIDs::R_Shoulder) || controller->GetButtonPressed(XButtonIDs::L_Shoulder))) {
        fireHook();
    }

    if (this->myHook->getStuck() && (controller->GetButtonPressed(XButtonIDs::R_Shoulder) || controller->GetButtonPressed(XButtonIDs::L_Shoulder))) {
        this->retracting = true;
    }

    if (this->retracting)
    {
        this->retractHook();
    }

    //Defines how soon the hook detaches
    float autoDetachLength = 400.f;

    this->myHook->update(pos + (HOOK_FORWARD_OFFSET*dir) + (HOOK_UP_OFFSET*up));
    if ((this->getHookDistance() > autoDetachLength) && (this->myHook->getShot())) {
        this->cancelHook();
    }
}



/*
^^
(0)|  ----  |(1)
||
||
||
(2)|  ----  |(3)

*/
void Car::applyWheelTurn(float factor) {
    double vdiff_2 = (this->mActor->getLinearVelocity().magnitude() / MAX_SPEED) * (this->mActor->getLinearVelocity().magnitude() / MAX_SPEED);
    this->mVehicleNoDrive->setSteerAngle(0, -factor / ((STEER_VEL_FACTOR * vdiff_2) + BASE_STEER));
    this->mVehicleNoDrive->setSteerAngle(1, -factor / ((STEER_VEL_FACTOR * vdiff_2) + BASE_STEER));

    //  this->mVehicleNoDrive->mWheelsDynData.pose
}

void Car::applyWheelTorque(float factor) {
    this->mVehicleNoDrive->setDriveTorque(0, -DRIVE_TORQUE*factor);
    this->mVehicleNoDrive->setDriveTorque(1, -DRIVE_TORQUE*factor);
    this->mVehicleNoDrive->setDriveTorque(2, -DRIVE_TORQUE*factor);
    this->mVehicleNoDrive->setDriveTorque(3, -DRIVE_TORQUE*factor);
}

void Car::resetBrakes()
{
    this->mVehicleNoDrive->setBrakeTorque(0, 0);
    this->mVehicleNoDrive->setBrakeTorque(1, 0);
    this->mVehicleNoDrive->setBrakeTorque(2, 0);
    this->mVehicleNoDrive->setBrakeTorque(3, 0);
}

void Car::startBrakeMode()
{
    this->mVehicleNoDrive->setBrakeTorque(0, BRAKE_TORQUE);
    this->mVehicleNoDrive->setBrakeTorque(1, BRAKE_TORQUE);
    this->mVehicleNoDrive->setBrakeTorque(2, BRAKE_TORQUE);
    this->mVehicleNoDrive->setBrakeTorque(3, BRAKE_TORQUE);
}

glm::vec3 Car::getAim() const {
    return aim;
}

glm::quat Car::getAimRot() const {
    return aim_rot;
}

VehicleDesc Car::initVehicleDesc()
{
    //Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
    //The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
    //Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.

    // TODO: Generalize these to be private class members (which we can modify at runtime with our config parser)

    const PxF32 chassisMass = CHASSIS_MASS;

    const PxVec3 chassisDims(CHASSIS_X, CHASSIS_Y, CHASSIS_Z);
    const PxVec3 chassisMOI
        ((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / CHASSIS_X_MOI_FACTOR,
            (chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)* 0.8f * chassisMass / CHASSIS_Y_MOI_FACTOR,
            (chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / CHASSIS_Z_MOI_FACTOR);
    const PxVec3 chassisCMOffset(CM_X, CM_Y, CM_Z); // COG -1

    const PxU32 nbWheels = NUM_WHEELS;

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
    tireMaterial = this->mPhysicsManager->mPhysics->createMaterial(MAT_STATIC, MAT_DYNAMIC, MAT_CR);
    mFrictionPairs = createFrictionPairs(tireMaterial, TIRE_FRICTION);

    vehicleDesc.wheelMaterial = tireMaterial; // This material doesn't affect the wheel's driving, but rather its non-driving
                                              // interactions with other surfaces (?)
    return vehicleDesc;
}

// Update the rendered position & rotation for each wheel
void Car::updateWheels(PxWheelQueryResult wheelQueryResults[NUM_WHEELS]) {
    PxTransform carPose = mActor->getGlobalPose();

    // Essentially copying PhysicsObject::updatePosAndRot()
    for (int i = 0; i < NUM_WHEELS; i++) {
        PxTransform wheelPose = wheelQueryResults[i].localPose;

        // Rotate by car rot
        glm::vec3 newPos = glm::rotate(qrot, glm::vec3(wheelPose.p.x, wheelPose.p.y, wheelPose.p.z));

        wheels[i]->setPos(carPose.p.x + newPos.x,
            carPose.p.y + newPos.y,
            carPose.p.z + newPos.z);

        PxQuat wheelRot;
        if (i % 2 == 0) { // left wheels
            wheelRot = carPose.q * wheelPose.q * L_WHEEL_MODEL_ROT;
        }
        else { // right wheels
            wheelRot = carPose.q * wheelPose.q * R_WHEEL_MODEL_ROT;
        }

        wheels[i]->setRot(glm::quat(wheelRot.w, wheelRot.x, wheelRot.y, wheelRot.z));
    }
}

void Car::stepForPhysics() {
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
    PxVehicleWheelQueryResult vehicleQueryResults = { wheelQueryResults, this->mVehicleNoDrive->mWheelsSimData.getNbWheels() };
    PxVehicleUpdates(1 / 60.f, grav, *mFrictionPairs, 1, vehicles, &vehicleQueryResults);

    // Updates the renderable positions for each wheel
    updateWheels(wheelQueryResults);
    // Perform physX update
    // MAX: Moved this here from update() so that the wheels would stop lagging behind. Probably makes more sense.
    updatePosandRot();
}

//Fires the hook
void Car::fireHook() {
    this->mPhysicsManager->mScene->addActor(*myHook->mActor);
    myHook->setShot(true);
    myHook->setRot(aim_rot);

    glm::vec3 b = arrow->getPos();
    myHook->setPos(b.x + (2.0f*aim.x), b.y + 2.0f, b.z + (2.0f*aim.z));
    //glm::vec3 a_pos = arrow->getPos();
    //myHook->setPos(a_pos.x, a_pos.y + 2.0f, a_pos.z);
}

void Car::cancelHook() {
    if (!this->myHook->getStuck())
        this->mPhysicsManager->mScene->removeActor(*myHook->mActor);
    myHook->setShot(false);
    myHook->setStuck(false);
    myHook->setPos(0.0, 200.0, 0.0);
    myHook->mActor->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));
    this->retracting = false;
}

void Car::retractHook() {

    PxVec3 launchDir = PxVec3(this->myHook->getPos().x, this->myHook->getPos().y, this->myHook->getPos().z) -
        PxVec3(pos.x, pos.y, pos.z);
    // Implement this again when cooldown is working
    /*if (launchDir.magnitude() < 30.f)
    {
    this->cancelHook();
    }*/

    launchDir.normalize();

    this->mActor->setLinearVelocity(this->mActor->getLinearVelocity() + launchDir * 10.f);
    this->mActor->setAngularVelocity(PxVec3(0.f, 0.f, 0.f));
}

double Car::getSpeed() {
    return this->mActor->getLinearVelocity().magnitude();
}

float Car::getHookDistance() {
    //retract hook
    vec3 launchDir = this->myHook->getPos() - pos;
    return glm::length(launchDir);
}
