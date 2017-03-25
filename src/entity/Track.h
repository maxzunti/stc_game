#pragma once
#include "StaticPhysicsObject.h"

class Track : public StaticPhysicsObject {
protected:
    const char* name;

public:
    Track(std::string model_fname, std::string tex_fname, glm::vec3 scale, PhysicsManager* physicsManager, PxU32 filterdata, PxU32 filterdataagainst);
    virtual Track::~Track();

};