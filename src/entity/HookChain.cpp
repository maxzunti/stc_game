#include "HookChain.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <iostream>
#include "PhysicsObject.h"

using namespace glm;


HookChain::HookChain(std::string model_fname, std::string tex_fname) :
    Renderable(model_fname, tex_fname) {
    scale(X_SCALE, Y_SCALE, Z_SCALE);
    rotate(base_rot.x, base_rot.y, base_rot.z);
    renderable = false;
}

void HookChain::reposition(glm::vec3 carPos, glm::vec3 hookPos) {
    if (enabled) {
        // Rescale to length
        getModel()->reset_scale();
        float len = glm::distance(hookPos, carPos);
        scale(X_SCALE, Y_SCALE * len, Z_SCALE);

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

void HookChain::enable(bool val) { enabled = val; renderable = val; }