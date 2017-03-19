#pragma once
#include "Renderable.h"

class HookChain : public Renderable {
    glm::quat base_rot = glm::quat(glm::vec3(M_PI/2.0f, 0.0f, 0.0f));
    glm::vec3 carPos;

    bool enabled = false;
    bool mStuck = false;

    Model* attached;
    Model* unattached;

public:
    HookChain(std::string model_fname, std::string tex_fname);
    ~HookChain();
    void reposition(glm::vec3 carPos, glm::vec3 hookPos);
    void enable(bool);
    void setStuck(bool);
   // void update(bool canRender, glm::vec3 carPos, glm::vec3 hookPos, glm::quat rot);
    glm::quat& getBaseRot();
    glm::vec3& getLocalCarPos();
};