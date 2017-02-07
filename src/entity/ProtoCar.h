#pragma once
#include "PhysicsObject.h"
#include "../input/input.h"

// Prototype for a car class - mostly used for testing some basic controller input,
// plus maybe generating some functions that'll be useful for an actual car later on
class ProtoCar : public PhysicsObject {

protected:
    Input * controller;

public:
    ProtoCar(std::string model_fname, std::string tex_fname, PxRigidBody* actor, Input * cont);

    virtual void update();

    virtual void applyGlobalForce(glm::vec3 direction, double magnitude);
    virtual void applyLocalForce(float forward, float right, float up);

    const double FORCE_FACTOR = 500.;
};