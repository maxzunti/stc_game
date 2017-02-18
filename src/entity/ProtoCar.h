#pragma once
#include "PhysicsObject.h"
#include "AimArrow.h"
#include "../input/input.h"
#include "PxPhysicsAPI.h"
#include "vehicle/PxVehicleUtil.h"
#include "vehicle/PxVehicleNoDrive.h"
#include "Hook.h"


#include "../Snippets/SnippetVehicleCommon/SnippetVehicleCreate.h"

// Prototype for a car class - mostly used for testing some basic controller input,
// plus maybe generating some functions that'll be useful for an actual car later on
class ProtoCar : public PhysicsObject {

protected:
    Input * controller;
    std::unique_ptr<AimArrow> arrow;

    /* (rendered) wheel array + index mappings
    (0)|  ----  |(1)
           ||
           ||
           ||
    (2)|  ----  |(3)
    */

    static const int NUM_WHEELS = 4;
    Renderable* wheels[NUM_WHEELS];
    static const int FL = 0;
    static const int FR = 1;
    static const int BL = 2;
    static const int BR = 3;
    const glm::quat glL_WHEEL_MODEL_ROT = glm::quat(glm::vec3(0.0f, 0.f, M_PI / 2.0f));
    const glm::quat glR_WHEEL_MODEL_ROT =  glm::quat(glm::vec3(0.0f, M_PI, 0.0f)) * glL_WHEEL_MODEL_ROT;
    const PxQuat L_WHEEL_MODEL_ROT = PxQuat(glL_WHEEL_MODEL_ROT.x, glL_WHEEL_MODEL_ROT.y, glL_WHEEL_MODEL_ROT.z, glL_WHEEL_MODEL_ROT.w);
    const PxQuat R_WHEEL_MODEL_ROT = PxQuat(glR_WHEEL_MODEL_ROT.x, glR_WHEEL_MODEL_ROT.y, glR_WHEEL_MODEL_ROT.z, glR_WHEEL_MODEL_ROT.w);
    const glm::vec3 WHEEL_MODEL_SCL = glm::vec3(0.5, 0.6, 0.5);

    glm::quat aim_rot;
    glm::vec3 aim;
    glm::vec3 up;
	std::unique_ptr<Hook> myHook;

    // Calculate an aim rotation using an xbox controller
    void calcAim();

    PxVehicleNoDrive*	mVehicleNoDrive = NULL;

    // Used to position the origin of the hook when fired from the car
    const float HOOK_FORWARD_OFFSET = 1.5;
    const float HOOK_UP_OFFSET = 0.8;

    // PhysX Wheel initializers
    // Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
    // Moment of inertia is just the moment of inertia of a cylinder.
    float WHEEL_MASS = 20.f; // 100
    float WHEEL_RAD = 0.5f;
    float WHEEL_WIDTH = 0.4f;
    float WHEEL_MOI = 0.1f * WHEEL_MASS * WHEEL_MASS * WHEEL_WIDTH; //0.1f*wheelRadius*wheelRadius;

    void initWheels(std::string model_fname, std::string tex_fname);
    void updateWheels(PxWheelQueryResult wheelQueryResults[NUM_WHEELS]);

public:
    ProtoCar(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, Input * cont, std::vector<Entity*> &ents);
    ~ProtoCar();

    VehicleDesc initVehicleDesc();


    virtual void update();

    virtual void applyGlobalForce(glm::vec3 direction, double magnitude);
    virtual void applyLocalForce(float forward, float right, float up);
    
    // why are these public?
    void ProtoCar::startBrakeMode();
    void ProtoCar::applyWheelTorque(float factor);
    void ProtoCar::applyWheelTurn(float factor);
    void ProtoCar::resetBrakes();

    // should probably use a different force factor for brake & forward torques
    const double FORCE_FACTOR = 10000.;

    const float MAX_SPEED = 60.f;

    bool retracting;

    glm::vec3 getAim() const;
    glm::quat getAimRot() const;

    void ProtoCar::stepForPhysics();

	void ProtoCar::fireHook();
    void ProtoCar::cancelHook();
    void ProtoCar::retractHook();
};