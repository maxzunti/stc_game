#pragma once
#include "StaticPhysicsObject.h"

class RectTrigger : public StaticPhysicsObject {
protected:
    bool triggered = false;

public:

    RectTrigger(PhysicsManager* physicsManager, std::string texName, float x_width, float y_width, float z_width, int id, glm::vec3 target, bool renderable = false);
    virtual RectTrigger::~RectTrigger();

    int triggerID;
    static const int HOOKZONE = 1000;
    static const int SWINGZONE = 1001;
    glm::vec3 target;
};