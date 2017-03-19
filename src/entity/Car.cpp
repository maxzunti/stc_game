#include "Car.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <math.h>

using namespace glm;

Car::Car(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, Input* cont, std::vector<Entity*> &ents, Jukebox* jb, StaticPhysicsObject* track) :
    DynamicPhysicsObject(model_fname, tex_fname, actor, physicsManager),
    arrow(new AimArrow("assets/models/AimArrow/AimArrow.obj", "assets/models/AimArrow/blue.png")),
    myHook(new Hook("assets/models/sphere/sphere.obj", "assets/models/sphere/blue.png", physicsManager->createHook(0.f, -5000.0f, 0.0f, 0.25f, 0.25f, 0.25f), physicsManager, ents)),
    car_parser("config/car_config", &carParams),
    hook_parser("config/hook_config", &hookParams)
{
    this->track = track;
    this->myJB = jb;
    physMan = physicsManager;

    initParams();
    initHookParams();

    car_parser.updateFromFile();
    hook_parser.updateFromFile();
    myHook->updateFromConfig();

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
    scale(0.7f, 1.0f, 1.0f);
    mActor->userData = this;
    mActor->setName("Car");
    this->lap = 1;
    this->partoflap = 0;
}


Car::Car(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents, StaticPhysicsObject* track) :
	DynamicPhysicsObject(model_fname, tex_fname, actor, physicsManager),
	arrow(new AimArrow("assets/models/AimArrow/AimArrow.obj", "assets/models/AimArrow/blue.png")),
	myHook(new Hook("assets/models/sphere/sphere.obj", "assets/models/sphere/blue.png", physicsManager->createHook(0.f, 100.0f, 0.0f, 0.25f, 0.25f, 0.25f), physicsManager, ents)),
	car_parser("config/car_config", &carParams),
    hook_parser("config/hook_config", &hookParams)
{
    this->track = track;
	physMan = physicsManager;

    initParams();
    initHookParams();

    car_parser.updateFromFile();
    hook_parser.updateFromFile();
    myHook->updateFromConfig();

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
    mActor->userData = this;
    mActor->setName("Car");
    this->lap = 1;
    this->partoflap = 0;
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
    carParams.push_back(std::make_pair(std::string("SLOWDOWN"), &SLOWDOWN));
    carParams.push_back(std::make_pair(std::string("SUSP_APPLY_OFFSET"), &SUSP_APPLY_OFFSET));
    carParams.push_back(std::make_pair(std::string("TIRE_APPLY_OFFSET"), &TIRE_APPLY_OFFSET));
    carParams.push_back(std::make_pair(std::string("MAX_STEER_SPEED"), &MAX_STEER_SPEED));
    carParams.push_back(std::make_pair(std::string("HOOK_FRICTION"), &HOOK_FRICTION));
    carParams.push_back(std::make_pair(std::string("LOW_SPEED"), &LOW_SPEED));
    carParams.push_back(std::make_pair(std::string("LOW_FRICTION"), &LOW_FRICTION));
    carParams.push_back(std::make_pair(std::string("MAX_STEER_ANGLE"), &MAX_STEER_ANGLE));
    carParams.push_back(std::make_pair(std::string("STEER_DECAY"), &STEER_DECAY));
}

void Car::initHookParams() {
    hookParams.push_back(std::make_pair(std::string("HOOK_PULL_SPEED"), &HOOK_PULL_SPEED));
    hookParams.push_back(std::make_pair(std::string("HOOK_BOOST_SPEED"), &HOOK_BOOST_SPEED));
    hookParams.push_back(std::make_pair(std::string("HOOK_ROT_FACTOR"), &HOOK_ROT_FACTOR));
    hookParams.push_back(std::make_pair(std::string("HOOK_MAX_LENGTH"), &HOOK_MAX_LENGTH));
    hookParams.push_back(std::make_pair(std::string("HOOK_MIN_LENGTH"), &HOOK_MIN_LENGTH));
    hookParams.push_back(std::make_pair(std::string("HOOK_START_DIST"), &HOOK_START_DIST));
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
    suspension.S_SUSP_APPLY_OFFSET = SUSP_APPLY_OFFSET;
    suspension.S_WHEEL_APPLY_OFFSET = TIRE_APPLY_OFFSET;

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
    float zero = 0;
    noFrictionPairs = createFrictionPairs(tireMaterial, zero);
    hookFrictionPairs = createFrictionPairs(tireMaterial, HOOK_FRICTION);
    lowFrictionPairs = createFrictionPairs(tireMaterial, LOW_FRICTION);

    vehicleDesc.wheelMaterial = tireMaterial; // This material doesn't affect the wheel's driving, but rather its non-driving
                                              // interactions with other surfaces (?)
    return vehicleDesc;
}


// Setup the wheels for rendering
void Car::initWheels(std::string model_fname, std::string tex_fname) {
    for (int i = 0; i < NUM_WHEELS; i++) {
        wheels[i] = new Wheel(model_fname, tex_fname);
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
    right = normalize(cross(dir, vec3(0, 1, 0)));
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

    if (this->getPos().y < -200.0f)
    {
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
            this->setRot(0.0, 1.57/2.0, 0.0);
            break;
        case 3:
            this->setPos(-988.594, 23, -207.866);
            this->setRot(0.0, 4.14 / 1.2, 0.0);
            break;
        default:
            std::cout << "switch error: you ded" << std::endl;
            break;
        }
    }
    if (controller->GetButtonPressed(XButtonIDs::X)) {
        car_parser.updateFromFile();
        hook_parser.updateFromFile();
        myHook->updateFromConfig();
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
        PxVec3 speed = this->mActor->getLinearVelocity();
        this->mActor->setLinearVelocity(speed * SLOWDOWN);
    }
    else if (this->mActor->getLinearVelocity().magnitude() > MAX_SPEED*1.5f && this->retracting)
    {
        PxVec3 temp = this->mActor->getLinearVelocity();
        temp.normalize();
        this->mActor->setLinearVelocity(MAX_SPEED * 1.5f * temp);
    }

    //Handbrake - Possibly remove in future
    if (controller->GetButtonPressed(XButtonIDs::A)) {
        //applyLocalForce(0, 0, 2000);
    //    startBrakeMode();
    }
    if (this->myHook->getStuck() && controller->GetButtonPressed(XButtonIDs::B)) {
        this->cancelHook();
    }

    // Update aim (after PhysX!)
    calcAim();
    arrow->reposition(up, pos, aim, aim_rot);

    if (myHook->getStuck()) {

        PxVec3 hookDir = PxVec3(myHook->getPos().x, myHook->getPos().y, myHook->getPos().z) -
            PxVec3(pos.x, pos.y, pos.z);
        float hookDist = hookDir.normalize();

        if (hookDist <= min_hookDist + 0.3) {
            min_hookDist = hookDist;
        }
        else { // min_hookDist < curr hook
            if (controller->GetButtonPressed(XButtonIDs::L_Shoulder)) {
                swinging = true;
            }
            else {
                cancelHook();
            }
        }
    }

    // Must fire after calc aim
    if ((!myHook->getShot() && !myHook->getStuck()) && (controller->GetButtonPressed(XButtonIDs::R_Shoulder) || controller->GetButtonPressed(XButtonIDs::L_Shoulder))) {
        fireHook();
    }

    if (myHook->getStuck() && (controller->GetButtonPressed(XButtonIDs::R_Shoulder) || controller->GetButtonPressed(XButtonIDs::L_Shoulder))) {
        retracting = true;

   //     this->myJB->playEffect(myJB->gravpull);
    }

    if (swinging) {
        swingHook();
    }
    else if (retracting)
    {
        retractHook();
    }

    //Defines the distance that the hook detaches at
    if (((getHookDistance() > HOOK_MAX_LENGTH) && (myHook->getShot())) || ((getHookDistance() < HOOK_MIN_LENGTH) && (myHook->getStuck()))) {
        cancelHook();
    }

    myHook->update(pos + (HOOK_FORWARD_OFFSET*dir) + (HOOK_UP_OFFSET*up));
   
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
   double vdiff_2 = (this->mActor->getLinearVelocity().magnitude() / MAX_STEER_SPEED) * (this->mActor->getLinearVelocity().magnitude() / MAX_STEER_SPEED);
   double f_2 = factor < 0 ? (factor * factor) : -(factor * factor);
   double f_3 = -factor * factor * factor;

   this->mVehicleNoDrive->setSteerAngle(0, f_3 / ((STEER_VEL_FACTOR * vdiff_2) + BASE_STEER));
   this->mVehicleNoDrive->setSteerAngle(1, f_3 / ((STEER_VEL_FACTOR * vdiff_2) + BASE_STEER));
 /*   PxReal currAngle = mVehicleNoDrive->getSteerAngle(0);
    PxReal newAngle = currAngle - (factor / ((STEER_VEL_FACTOR * vdiff_2) + BASE_STEER));
    float LS_steer_deadzone = 0.25;
    bool LS_used = (controller->LeftStick_X() > LS_steer_deadzone || controller->LeftStick_X() < -LS_steer_deadzone) ||
                   (controller->LeftStick_Y() > LS_steer_deadzone || controller->LeftStick_Y() < -LS_steer_deadzone);
    std::cout << "LS_used = " << LS_used << std::endl;
    std::cout << "LSX = " << controller->LeftStick_X() << "    LSY = " << controller->LeftStick_Y() << std::endl;
    if (!LS_used && mActor->getLinearVelocity().magnitude() > 1) {
        if (newAngle > STEER_DECAY) {
            newAngle -= STEER_DECAY;
        }
        else if (newAngle < -STEER_DECAY) {
            newAngle += STEER_DECAY;
        }
        else {
            newAngle = 0;
        }
    }

    if (newAngle > MAX_STEER_ANGLE) {
        newAngle = MAX_STEER_ANGLE;
    }
    else if (newAngle < -MAX_STEER_ANGLE) {
        newAngle = -MAX_STEER_ANGLE;
    }
    mVehicleNoDrive->setSteerAngle(0, newAngle);
    mVehicleNoDrive->setSteerAngle(1, newAngle);*/

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

        glm::quat inv_local = glm::inverse(glm::quat(wheelPose.q.w, wheelPose.q.x, wheelPose.q.y, wheelPose.q.z));
        PxQuat invRot = PxQuat(inv_local.x, inv_local.y, inv_local.z, inv_local.w);
        PxQuat wheelRot;

        if (i % 2 == 0) { // left wheels
            wheelRot = carPose.q * wheelPose.q * L_WHEEL_MODEL_ROT;
            invRot = carPose.q * invRot * L_WHEEL_MODEL_ROT;
        }
        else { // right wheels
            wheelRot = carPose.q * wheelPose.q * R_WHEEL_MODEL_ROT;
            invRot = carPose.q * invRot * R_WHEEL_MODEL_ROT;
        }
        wheels[i]->set_inv_rot(glm::quat(invRot.w, invRot.x, invRot.y, invRot.z));
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
    if (mActor->getLinearVelocity().magnitude() < LOW_SPEED) {
        PxVehicleUpdates(1 / 60.f, grav, *lowFrictionPairs, 1, vehicles, &vehicleQueryResults); // use low-friction at low speeds to stop jittering
    }
    else {
        if (swinging) {
            PxVehicleUpdates(1 / 60.f, grav, *hookFrictionPairs, 1, vehicles, &vehicleQueryResults);
        }
        else if (retracting) { // disable friction when retracting
            PxVehicleUpdates(1 / 60.f, grav, *hookFrictionPairs, 1, vehicles, &vehicleQueryResults);
        }
        else {
            PxVehicleUpdates(1 / 60.f, grav, *mFrictionPairs, 1, vehicles, &vehicleQueryResults);
        }
    }

    // Updates the renderable positions for each wheel
    updateWheels(wheelQueryResults);
    // Perform physX update
    // MAX: Moved this here from update() so that the wheels would stop lagging behind. Probably makes more sense.
    updatePosandRot();
}

//Fires the hook
void Car::fireHook() {
    min_hookDist = 999999.0f;

  //  this->myJB->playEffect(myJB->firehook);
    this->mPhysicsManager->mScene->addActor(*myHook->mActor);
    myHook->setShot(true);
    myHook->setRot(aim_rot);

    glm::vec3 b = arrow->getPos();
    myHook->setPos(b.x + (HOOK_START_DIST*aim.x), b.y + HOOK_START_DIST, b.z + (HOOK_START_DIST*aim.z));
    //glm::vec3 a_pos = arrow->getPos();
    //myHook->setPos(a_pos.x, a_pos.y + 2.0f, a_pos.z);
}

void Car::cancelHook() {
    if (!this->myHook->getStuck())
        this->mPhysicsManager->mScene->removeActor(*myHook->mActor);
    myHook->setShot(false);
    myHook->setStuck(false);
    myHook->setPos(0.0, -5000.0, 0.0);
    myHook->mActor->setLinearVelocity(PxVec3(0.f, 0.f, 0.f));
    retracting = false;
    swinging = false;
}

void Car::retractHook() {
    PxVec3 carDir = PxVec3(dir.x, dir.y, dir.z);
    PxVec3 hookDir = PxVec3(myHook->getPos().x, myHook->getPos().y, myHook->getPos().z) -
        PxVec3(pos.x, pos.y, pos.z);
    hookDir.normalize();

   mActor->setLinearVelocity(this->mActor->getLinearVelocity() + (hookDir * HOOK_PULL_SPEED) + (carDir * HOOK_BOOST_SPEED));
}

void Car::swingHook() {
    float speed = mActor->getLinearVelocity().magnitude();
    if (speed < MAX_SPEED) {
    //    speed += 1.5f;
    }

    PxVec3 hookDir = PxVec3(myHook->getPos().x, myHook->getPos().y, myHook->getPos().z) -
        PxVec3(pos.x, pos.y, pos.z);
    float angle = mActor->getLinearVelocity().magnitude() / (60.0f * hookDir.magnitude()); // theta = (s / r) = (v / 60 R) => angle per frame
    vec3 hookRad(hookDir.x, hookDir.y, hookDir.z);

  //  vec3 newPos = pos;
    if (dot(hookRad, right) > 0) { // on the right
  //      newPos -= hookRad * (1 - cos(angle));
  //      newPos += glm::cross(hookRad, up) * sin(-angle);
        rotateAboutUp(-angle);
    }
    else {
 //       newPos += hookRad * (1 - cos(angle));
 //       newPos += glm::cross(hookRad, up) * sin(angle);
        rotateAboutUp(angle);
    }

    mActor->setLinearVelocity(PxVec3(dir.x * speed, dir.y * speed, dir.z * speed));
}


double Car::getSpeed() {
    return this->mActor->getLinearVelocity().magnitude();
}

float Car::getHookDistance() {
    //retract hook
    vec3 launchDir = this->myHook->getPos() - pos;
    return glm::length(launchDir);
}

int Car::getLap() {
    return lap;
}

int Car::getPartOfLap() {
    return partoflap;
}

void Car::rotateAboutUp(float angle) {
    qrot = glm::rotate(qrot, angle, up);
    setRot(qrot);
}

bool Car::isCar() {
    return true;
}

// Perform a downwards raycast to get a height above the track
RaycastResults Car::doRaycast() {
    PxReal maxDistance = 500;            // [in] Raycast max distance
    PxRaycastBuffer hit;                 // [out] Raycast results
    mPhysicsManager->mScene;
    float distance = -1.0;
    RaycastResults raycast;

    const PxU32 bufferSize = 256;
    PxRaycastHit hitBuffer[bufferSize];
    PxRaycastBuffer buf(hitBuffer, bufferSize);
    if (mPhysicsManager->mScene->raycast(PxVec3(pos.x, pos.y, pos.z), PxVec3(0.0, -1.0, 0.0), maxDistance, buf)) {
        for (PxU32 i = 0; i < buf.nbTouches; i++) {
            if (buf.touches[i].actor->getName() == "track") {
                raycast.distance = buf.touches[i].distance;
                raycast.normal = glm::vec3(buf.touches[i].normal.x, buf.touches[i].normal.y, buf.touches[i].normal.z);
            }
        }
    }
    return raycast;
}

// Perform a downwards raycast to get a height above the track
RaycastResults Car::doHookRaycast() {
    RaycastResults raycast;
    if (!myHook->getShot()) {
        return raycast;
    }
    PxReal maxDistance = 500;            // [in] Raycast max distance
    PxRaycastBuffer hit;                 // [out] Raycast results
    mPhysicsManager->mScene;
    float distance = -1.0;
    PxVec3 pos = PxVec3(myHook->xPos(), myHook->yPos(), myHook->zPos());

    const PxU32 bufferSize = 256;
    PxRaycastHit hitBuffer[bufferSize];
    PxRaycastBuffer buf(hitBuffer, bufferSize);
    if (mPhysicsManager->mScene->raycast(PxVec3(pos.x, pos.y, pos.z), PxVec3(0.0, -1.0, 0.0), maxDistance, buf)) {
        for (PxU32 i = 0; i < buf.nbTouches; i++) {
            if (buf.touches[i].actor->getName() == "track") {
                raycast.distance = buf.touches[i].distance;
                raycast.normal = glm::vec3(buf.touches[i].normal.x, buf.touches[i].normal.y, buf.touches[i].normal.z);
            }
        }
    }
    return raycast;
}


Wheel* Car::getWheel(int index) {
    return wheels[index];
}

glm::vec3& Car::getRight() {
    return right;
}

glm::vec3& Car::getUp() {
    return up;
}

Hook * Car::getHook() {
    return myHook.get();
}
