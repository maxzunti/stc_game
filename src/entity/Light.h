#pragma once

#include "Entity.h"

class Light: public Entity {
protected:
    glm::vec3 pos; // x, y, z
    glm::vec3 dir; // forward-facing direction
public:
    Light(glm::vec3 ipos, glm::vec3 idir);
    ~Light();

    glm::vec3& getPos();
    glm::vec3 getDir();
    
    void setPos(double x, double y, double z);
    void setPos(glm::vec3 newPos);

};