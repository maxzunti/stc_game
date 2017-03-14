#pragma once
#include "DynamicPhysicsObject.h"
#include "../Snippets/SnippetVehicleCommon/SnippetVehicleFilterShader.h"
#include "../util/ConfigParser.h"
#include "HookChain.h"

class Hook : public DynamicPhysicsObject {
protected:
    Model* unattached;
    Model* attached;
    bool mStuck;
    bool mShot;
    float HOOK_SPEED = 500.0f;

    ConfigParser hook_parser;
    fp_vars hookParams;

    void initHookParams();

public:
	Hook(std::string model_fname, std::string tex_fname, physx::PxRigidBody* actor, PhysicsManager* physicsManager, std::vector<Entity*> &ents);
    ~Hook();
    std::unique_ptr<HookChain> chain;

	virtual void update(glm::vec3 carPos);

	virtual void applyGlobalForce(glm::vec3 direction, double magnitude);
	virtual void applyLocalForce(float forward, float right, float up);

    void setStuck(bool val);
    void setShot(bool val);
    bool getStuck();
    bool getShot();
    void updateFromConfig();
};