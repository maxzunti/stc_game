#pragma once
#include "DynamicPhysicsObject.h"
#include "StaticPhysicsObject.h"

#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleUtil.h"
#include "vehicle/PxVehicleNoDrive.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleCreate.h"

#include "AimArrow.h"
#include "../input/input.h"
#include "Hook.h"
#include "Wheel.h"
#include "RectTrigger.h"

#include "../util/ConfigParser.h"


#include "../Jukebox.h"
struct RaycastResults {
    glm::vec3 normal;
    float distance = -1.0f;
};

enum CarColor {
        PURPLE = 0,
        RED,
        LB,
        ORANGE
};

struct CarRenderInfo {
    static const char* getTex(CarColor);
    static const char* getMinimapIndex(CarColor);
};

// Prototype for a car class - mostly used for testing some basic controller input,
// plus maybe generating some functions that'll be useful for an actual car later on
class Car : public DynamicPhysicsObject {

public:
    Car(CarColor col, std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, Input * cont, std::vector<Entity*> &ents, Jukebox* jb, StaticPhysicsObject * track, std::vector<RectTrigger*> AInodes);
	Car(CarColor col, std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents, StaticPhysicsObject* track, std::vector<RectTrigger*> AInodes);
    ~Car();

    int lap;
    int partoflap;
    static const int NUM_WHEELS = 4;

    VehicleDesc initVehicleDesc();
    CarColor color;
    virtual void update();

    virtual void applyGlobalForce(glm::vec3 direction, double magnitude);
    virtual void applyLocalForce(float forward, float right, float up);
    virtual bool isCar(); // yes, it is.

    glm::vec3 getAim() const;
    glm::quat getAimRot() const;
    double getSpeed();
    float Car::getHookDistance();
    int Car::getLap();
    int Car::getPartOfLap();
    void Car::stepForPhysics();
    virtual void rotateAboutUp(float angle);
    virtual RaycastResults doRaycast();
    RaycastResults doHookRaycast();
    Wheel* getWheel(int index); // return a pointer to a given wheel
    glm::vec3& getRight();
    glm::vec3& getUp();
    Hook * getHook();
    std::vector<RectTrigger*> nodes;

    bool devChange = true;

protected:
    Input * controller;
    PhysicsManager * physMan;
    std::unique_ptr<AimArrow> arrow;
    StaticPhysicsObject * track;
    Jukebox* myJB;

    ConfigParser car_parser;
    ConfigParser hook_parser;

    glm::quat aim_rot;
    glm::vec3 aim;
    glm::vec3 up;
    glm::vec3 right;
    std::unique_ptr<Hook> myHook;
    bool retracting = false;
    bool swinging = false;
    float min_hookDist; // min hook distance

    PxVehicleNoDrive*	mVehicleNoDrive = NULL;
    PxVehicleDrivableSurfaceToTireFrictionPairs * mFrictionPairs = NULL;
    PxVehicleDrivableSurfaceToTireFrictionPairs * noFrictionPairs = NULL;
    PxVehicleDrivableSurfaceToTireFrictionPairs * lowFrictionPairs = NULL;
    PxVehicleDrivableSurfaceToTireFrictionPairs * hookFrictionPairs = NULL;

    PxMaterial* tireMaterial;

    /* (rendered) wheel array + index mappings
    (0)|  ----  |(1)
    ||
    ||
    ||
    (2)|  ----  |(3)
    */

    Wheel* wheels[NUM_WHEELS];
    static const int FL = 0;
    static const int FR = 1;
    static const int BL = 2;
    static const int BR = 3;
    const glm::quat glL_WHEEL_MODEL_ROT = glm::quat(glm::vec3(0.0f, 0.f, M_PI / 2.0f));
    const glm::quat glR_WHEEL_MODEL_ROT = glm::quat(glm::vec3(0.0f, M_PI, 0.0f)) * glL_WHEEL_MODEL_ROT;
    const PxQuat L_WHEEL_MODEL_ROT = PxQuat(glL_WHEEL_MODEL_ROT.x, glL_WHEEL_MODEL_ROT.y, glL_WHEEL_MODEL_ROT.z, glL_WHEEL_MODEL_ROT.w);
    const PxQuat R_WHEEL_MODEL_ROT = PxQuat(glR_WHEEL_MODEL_ROT.x, glR_WHEEL_MODEL_ROT.y, glR_WHEEL_MODEL_ROT.z, glR_WHEEL_MODEL_ROT.w);

    const glm::vec3 WHEEL_MODEL_SCL = glm::vec3(0.7, 1.5, 0.7);

    // Used to position the origin of the hook when fired from the car
    const float HOOK_FORWARD_OFFSET = 1.5;
    const float HOOK_UP_OFFSET = 0.8;

    // Config-modifiable parameters go here
    fp_vars carParams;
    fp_vars hookParams;

    float CHASSIS_MASS = 1500.0f;
    float CHASSIS_X = 2.5f;
    float CHASSIS_Y = 2.0f;
    float CHASSIS_Z = 5.0f;
    float CHASSIS_X_MOI_FACTOR = 12.0f;
    float CHASSIS_Y_MOI_FACTOR = 12.0f;
    float CHASSIS_Z_MOI_FACTOR = 12.0f;

    // PhysX Wheel initializers
    // Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    // Moment of inertia is just the moment of inertia of a cylinder.
    float WHEEL_MASS = 20.f; // 100
    float WHEEL_RAD = 0.5f;
    float WHEEL_WIDTH = 0.4f;
    float WHEEL_MOI = 0.1f * WHEEL_MASS * WHEEL_RAD * WHEEL_RAD; //0.1f*wheelRadius*wheelRadius;
    float WHEEL_DAMPING_RATE = 0.25;
    float WHEEL_MAX_BRAKE_TORQUE = 1500.0f;
    float WHEEL_MAX_STEER = 0.0f;
    float WHEEL_TOE_ANGLE = 0.0f;

    // Wheel position offsets
    float WHEEL_FRONT_Z = 0.3f;
    float WHEEL_BACK_Z = 0.3f;
    float WHEEL_Y_LOWER = 0.0f;
    float WHEEL_X_FACTOR = 0.5f;
    float WHEEL_Z_FACTOR = 0.5f;

    // Tire data
    float TIRE_FRICTION = 100000.f;
    float HOOK_FRICTION = 0.0f; // friction while hooked
    float LAT_STIFF_X = 2.0f;
    float LAT_STIFF_Y = 0.3125f * (180.0f / PxPi);
    float LONG_STIFF_PER_UNIT_GRAV = 1000.0f;
    float LOW_SPEED = 5.0f;
    float LOW_FRICTION = 1.0f;

    // Friction-slip graph tire data
    float G_FRIC_AT_ZERO_LONG_SLIP = 1.0f;
    float G_LONG_SLIP_W_MAX_FRICTION = 0.1f;
    float G_MAX_FRICTION = 1.0f;
    float G_END_POINT = 1.0f;
    float G_FRIC_PAST_END = 1.0f;

    // Driving paramters
    float STEER_VEL_FACTOR = 15.0f; // controls how much we can steer as a factor of current velocity
    float MAX_STEER_ANGLE = 0.3;
    float STEER_DECAY = 0.02;
    float MAX_STEER_SPEED = 250.0f;
    float BASE_STEER = 1.0f;
    float DRIVE_TORQUE = 10000.0f;
    float BRAKE_TORQUE = DRIVE_TORQUE * 100000.0f;
    float MAX_SPEED = 60.f;
    float SLOWDOWN = 2.0f; // determines how much we slow down per frame

                           // material-related values (supposedly, these won't actually affect our tire driving-friction, but we include them as part of the config anyway)
    float MAT_STATIC = 0.0f;
    float MAT_DYNAMIC = 0.0f;
    float MAT_CR = 0.0f; // coefficient of restituion

                         // center-of-mass offsets
    float CM_X = 0.0f;
    float CM_Y = 0.65f - 1.5f;
    float CM_Z = 0.25f;

    // Suspension-related values
    float MAX_COMPRESSION = 0.3;
    float MAX_DROOP = 0.1;
    float SPRING_STRENGTH = 3500000.f;
    float SPRING_DAMPER_RATE = 4500.f;
    float SUSP_APPLY_OFFSET = -0.3f;
    float TIRE_APPLY_OFFSET = -0.3f;

    // Hook parameters, read from 'hook_config'
    float HOOK_PULL_SPEED = 5.0f;
    float HOOK_BOOST_SPEED = 5.0f;
    float HOOK_ROT_FACTOR = 100.0f;
    float HOOK_MAX_LENGTH = 400.f;
    float HOOK_MIN_LENGTH = 75.f;
    float HOOK_START_DIST = 5.0f;

    void initWheels(std::string model_fname, std::string tex_fname);
    void updateWheels(PxWheelQueryResult wheelQueryResults[NUM_WHEELS]);

    void initParams();
    void initHookParams();

    // Calculate an aim rotation using an xbox controller
    void calcAim();

    void startBrakeMode();
    void applyWheelTorque(float factor);
    void applyWheelTurn(float factor);
    void resetBrakes();

    void fireHook();
    void cancelHook();
    void retractHook();
    void swingHook();

    void make_physX_car();
};