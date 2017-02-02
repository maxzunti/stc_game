#pragma once
#include "PhysicsObject.h"

class DynamicPhysicsObject : public PhysicsObject {
protected:

public:
    void applyForce(vec3 direction, double magnitude);
};