#include "HookChain.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include "PhysicsObject.h"

using namespace glm;

HookChain::HookChain(std::string model_fname, std::string tex_fname) :
    Renderable(model_fname, tex_fname) {

    // At this point, we've loaded a single hook model using the PhysicsObject constructor
    assert(models.size() == 1);
    unattached = models[0];
    attached = new Model(model_fname, "assets/models/cylinder/oline.png");

    models.push_back(attached);
    this->X_MODEL_SCALE = 0.3;
    this->Y_MODEL_SCALE = 0.2155181703369411;
    this->Z_MODEL_SCALE = 0.3;
    this->SIL_X_SCALE = 1.4;
    this->SIL_Z_SCALE = 1.4;
    this->SIL_Y_SCALE = 1.0;
    scaleModels();
    models.pop_back();

    rotate(base_rot.x, base_rot.y, base_rot.z);
    renderable = false;
}

HookChain::~HookChain() {
    // Delete whichever model isn't in 'models' (since Renderable's destructor will catch anything that is)
    if (mStuck)
        delete unattached;
    else
        delete attached;
}

void HookChain::reposition(glm::vec3 carPos, glm::vec3 hookPos) {
    if (enabled) {
        // Rescale to length
        reset_scale();
        float len = glm::distance(hookPos, carPos);
        scale(1., len, 1.);

        attached->tile_UV_Y(Y_MODEL_SCALE * len);
        unattached->tile_UV_Y(Y_MODEL_SCALE * len);

        // Set position between car and hook
        vec3 dir = normalize(hookPos - carPos);
        pos = carPos + (len / 2.0f * dir);

        // Determine a (world-absolute) rotation axis by crossing dir w/ the world default
        vec3 rot_axis = normalize(cross(vec3(0.0f, 0.0f, -1.0f), dir));

        // Calculate the angle to rotate by
        float angle = acos(-dir.z);

        // Rotate by base_rot (.obj model-dependent), then by the angle around the rotation axis
        quat none = quat(vec3(0.0f, 0.0f, 0.0f));
        qrot = glm::rotate(none, angle, rot_axis) * base_rot;
    }
}

void HookChain::enable(bool val) {
    enabled = val;
    renderable = val;
}

void HookChain::setStuck(bool val) {
    mStuck = val;
    if (mStuck) {
        models[0] = attached;
    }
    else {
        models[0] = unattached;
    }
}