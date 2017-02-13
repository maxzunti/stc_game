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
    glm::quat aim_rot;
    glm::vec3 aim;
    glm::vec3 up;
	std::unique_ptr<Hook> myHook;

    // Calculate an aim rotation using an xbox controller
    void calcAim();

    PxVehicleNoDrive*	mVehicleNoDrive = NULL;

public:
    ProtoCar(std::string model_fname, std::string tex_fname, PxRigidBody* actor, PhysicsManager* physicsManager, Input * cont, std::vector<Entity*> &ents);

    VehicleDesc initVehicleDesc();


    virtual void update();

    virtual void applyGlobalForce(glm::vec3 direction, double magnitude);
    virtual void applyLocalForce(float forward, float right, float up);
    
    void ProtoCar::startBrakeMode();
    void ProtoCar::applyWheelTorque(float factor);
    void ProtoCar::applyWheelTurn(float factor);
    void ProtoCar::resetBrakes();

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