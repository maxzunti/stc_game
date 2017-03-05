#pragma once
#include "StaticPhysicsObject.h"

class Trigger : public StaticPhysicsObject {
protected:
    bool triggered = false;

public:
    Trigger(std::string model_fname, std::string tex_fname, PhysicsManager* physicsManager);
};