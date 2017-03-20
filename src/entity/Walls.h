#pragma once
#include "StaticPhysicsObject.h"

class Walls : public StaticPhysicsObject {
protected:

public:
    float height = 0.0; // determines offset for drawing reflections
    Walls(std::string model_fname, std::string tex_fname, glm::vec3 scale, PhysicsManager* physicsManager, PxU32 filterdata, PxU32 filterdataagainst);
};