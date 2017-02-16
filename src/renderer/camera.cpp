#include "camera.h"
#include <cstdio>
#include <iostream>
#include <glm/gtx/quaternion.hpp>

using namespace glm;

mat4 rotateAbout(vec3 axis, float radians)
{
	mat4 matrix;

	matrix[0][0] = cos(radians) + axis.x*axis.x*(1-cos(radians));
	matrix[1][0] = axis.x*axis.y*(1-cos(radians))-axis.z*sin(radians);
	matrix[2][0] = axis.x*axis.z*(1-cos(radians)) + axis.y*sin(radians);

	matrix[0][1] = axis.y*axis.x*(1-cos(radians)) + axis.z*sin(radians);
	matrix[1][1] = cos(radians) + axis.y*axis.y*(1-cos(radians));
	matrix[2][1] = axis.y*axis.z*(1-cos(radians)) - axis.x*sin(radians);

	matrix[0][2] = axis.z*axis.x*(1-cos(radians)) - axis.y*sin(radians);
	matrix[1][2] = axis.z*axis.y*(1-cos(radians)) + axis.x*sin(radians);
	matrix[2][2] = cos(radians) + axis.z*axis.z*(1-cos(radians));

	return matrix;
}
/*
Camera::Camera():	dir(vec3(0, 0, -1)), 
					right(vec3(1, 0, 0)), 
					up(vec3(0, 1, 0)),
					pos(vec3(0, 0, 0)),
					zoom(0.f)
{

}*/

Camera::Camera(vec3 _dir, vec3 _pos) 
    : dir(normalize(_dir)), pos(_pos),
      fc_parser("config/follow_cam", &follow_vars)
{
	right = normalize(cross(_dir, vec3(0, 1, 0)));
	up =  normalize(cross(right, _dir));
	zoom = -4.0f;
    
    // Initialize follow-cam delay buffers
    for (int i = 0; i < FOLLOW_DELAY_SIZE; i++) {
        prev_rot[i] = glm::quat();
        prev_pos[i] = glm::vec3();
    }

    follow_vars.push_back(std::make_pair(std::string("FOLLOW_X_CAM_ROT_SPEED"), &FOLLOW_X_CAM_ROT_SPEED));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_Y_CAM_ROT_SPEED"), &FOLLOW_Y_CAM_ROT_SPEED));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_X_CAM_XBOX_SPEED"), &FOLLOW_X_CAM_XBOX_SPEED));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_Y_CAM_XBOX_SPEED"), &FOLLOW_Y_CAM_XBOX_SPEED));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_X_MAX_ROT"), &FOLLOW_X_MAX_ROT));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_Y_MAX_ROT"), &FOLLOW_Y_MAX_ROT));
    follow_vars.push_back(std::make_pair(std::string("SNAP_X"), &SNAP_X));
    follow_vars.push_back(std::make_pair(std::string("SNAP_Y"), &SNAP_Y));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_X_MAX_ROT_SPEED"), &FOLLOW_X_MAX_ROT_SPEED));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_Y_MAX_ROT_SPEED"), &FOLLOW_Y_MAX_ROT_SPEED));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_X_MAX_XBOX_SPEED"), &FOLLOW_X_MAX_XBOX_SPEED));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_Y_MAX_XBOX_SPEED"), &FOLLOW_Y_MAX_XBOX_SPEED));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_X_CAM_RETURN_SPEED"), &FOLLOW_X_CAM_RETURN_SPEED));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_Y_CAM_RETURN_SPEED"), &FOLLOW_Y_CAM_RETURN_SPEED));
    follow_vars.push_back(std::make_pair(std::string("X_ASYMP_FACTOR"), &X_ASYMP_FACTOR));
    follow_vars.push_back(std::make_pair(std::string("Y_ASYMP_FACTOR"), &Y_ASYMP_FACTOR));
    follow_vars.push_back(std::make_pair(std::string("X_ASYMP_RET_FACTOR"), &X_ASYMP_RET_FACTOR));
    follow_vars.push_back(std::make_pair(std::string("Y_ASYMP_RET_FACTOR"), &Y_ASYMP_RET_FACTOR));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_DISTANCE"), &FOLLOW_DISTANCE));
    follow_vars.push_back(std::make_pair(std::string("FOLLOW_HEIGHT"), &FOLLOW_HEIGHT));
    follow_vars.push_back(std::make_pair(std::string("BASE_ANGLE"), &BASE_ANGLE));
    follow_vars.push_back(std::make_pair(std::string("DELTA"), &DELTA));
    fc_parser.updateFromFile();
}

/*
	[ Right 0 ]
	[ Up 	0 ]
	[ -Dir	0 ]
	[ 0 0 0 1 ]
*/

void Camera::setMode() {
    static int framecount = 0; // hacky workaround - remove me at some point
    framecount++;
    // Toggle camera mode
    if (controller->GetButtonPressed(XButtonIDs::Y) && framecount >= 10) {
        framecount = 0;
        if (mode == camMode::FOLLOW) {
            mode = camMode::FREE;
        }
        else if (mode == camMode::FREE) {
            mode = camMode::FOLLOW;
        }
    }
}


mat4 Camera::getMatrix()
{
    mat4 cameraRotation = mat4(
        vec4(right, 0),
        vec4(up, 0),
        vec4(-dir, 0),
        vec4(0, 0, 0, 1));

    mat4 translation = mat4(
        vec4(1, 0, 0, 0),
        vec4(0, 1, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(-pos, 1));

    return transpose(cameraRotation)*translation;
}

void Camera::rotateAroundCenter(float x, float y, vec3 focus)
{
    vec3 camFocus = pos - focus;
    mat4 rotateAroundUp = rotateAbout(vec3(0, 1, 0), (zoom <= -1) ? x*abs(zoom) : x);
    mat4 rotateAroundRight = rotateAbout(right, (zoom <= -1) ? y*abs(zoom) : y);

    vec3 posRelOrigin = rotateAroundUp * rotateAroundRight * vec4(camFocus, 0);
    posRelOrigin += camFocus;

    vec3 normalized = normalize(posRelOrigin);
    //	(normalized.y > 0.65)? normalized.y = 0.65:normalized.y;
    //	(normalized.y < -0.65)? normalized.y = -0.65:normalized.y;

    pos = normalized + dir*zoom;
    dir = -1.f*normalize(camFocus);
    right = normalize(cross(dir, vec3(0, 1, 0)));
    up = normalize(cross(right, dir));
}

void Camera::rotateCamera(float x, float y)
{
    mat4 rotateAroundY = rotateAbout(vec3(0, 1, 0), x);
    mat4 rotateAroundX = rotateAbout(right, y);

    dir = normalize(rotateAroundX * rotateAroundY * vec4(dir, 0));
    right = normalize(cross(dir, vec3(0, 1, 0)));
    up = normalize(cross(right, dir));
}

void Camera::movePosition(glm::vec3 delta) {
    pos += delta;
}

void Camera::quatRot(glm::tquat<double> q) {
    glm::tvec3<double> base(0, 0, -1);

    dir = glm::rotate(q, base);
    right = normalize(cross(dir, vec3(0, 1, 0)));
    up = normalize(cross(right, dir));
}

void Camera::registerController(Input * newCont) {
    controller = newCont;
}

void Camera::registerCar(ProtoCar * newCar) {
    car = newCar;
}

void Camera::freeLook() {
    // Perform camera rotations
    if (!controller->RStick_InDeadzone()) {
        rotateCamera(-controller->RightStick_X() * FREE_X_CAM_ROT_SPEED,
                     controller->RightStick_Y() * FREE_Y_CAM_ROT_SPEED);
    }

    // free forward + lateral movement
    if (!controller->LStick_InDeadzone()) {
        movePosition((right * (controller->LeftStick_X() * FREE_X_CAM_MOVE_SPEED)) +
                     (dir * (controller->LeftStick_Y() * FREE_Y_CAM_MOVE_SPEED)));
    }

    // vertical movement
    if (controller->GetButtonDown(XButtonIDs::L_Shoulder)) {
         movePosition(up * -FREE_Z_CAM_MOVE_SPEED);
    }
    if (controller->GetButtonDown(XButtonIDs::R_Shoulder)) {
        movePosition(up * FREE_Z_CAM_MOVE_SPEED);
    }
}

void Camera::followLook() {

    // Calculate camera rotation
    glm::quat camQ(glm::vec3(x_cam_rot, y_cam_rot, 0.0f));
    glm::quat base(glm::vec3(BASE_ANGLE, 0.0f, 0.0f));

    // Rotate the cars' direction by the camera offset
   // camQ = car->getQRot() * base * camQ; 
     camQ = prev_rot[frame_counter % FOLLOW_DELAY_ROT] * base * camQ;

    glm::vec3 newDir = glm::rotate(camQ, vec3(0, 0, -1)); // calc new direction vector

    // Offset camera based on given direction; update cam direction
    // pos = car->getPos() - (FOLLOW_DISTANCE * newDir);
    pos = prev_pos[frame_counter % FOLLOW_DELAY_POS] - (FOLLOW_DISTANCE * newDir);
    pos.y += FOLLOW_HEIGHT;
    quatRot(camQ);
}

void Camera::update() {
    // Update camera position
    if (controller) {
        if (controller->GetButtonPressed(XButtonIDs::X)) {
            fc_parser.updateFromFile();
        }

        setMode();
        if (mode == camMode::FREE) {
            freeLook();
        }
        else if (car && mode == camMode::FOLLOW) {
            followLook();
        }
    }
    calcFollowSpeeds();


    prev_rot[frame_counter % FOLLOW_DELAY_ROT] = car->getQRot();
    prev_pos[frame_counter % FOLLOW_DELAY_POS] = car->getPos();

//    std::cout << "prev y = " << prev_x << std::endl;
   // std::cout << "diff: " << glm::rotate(car->getQRot() * glm::inverse(prot), 0.0f, up).y << std::endl;

  //  glm::inverse(car->getQRot));
  //  prev_x = (car->getQRot() * glm::rotate(car->getQRot(), 0.0f, up)).y;
    p_rot = car->getQRot();
    frame_counter++;
}

// Clamps a given speed to a max value
void Camera::clamp(float &input, float abs_max) {
    if (input >= abs_max) {
        input = abs_max;
    }
    else if (input <= -abs_max) {
        input = -abs_max;
    }
}

// Asymptotically clamps a speed based on nearness to threshold values
void Camera::soft_clamp_speed(const float &curr, float &speed, float max, float min, float factor) {
    if (speed < 0) { // negative speed; check min
        float diff = curr - min;
        speed *= factor * diff * diff; // quad scaling w/ diff (little diffs mean more)
    } else {
        float diff = max - curr;
        speed *= factor * diff * diff;
    }
}

// Brings values close to the target by 'step'
void Camera::converge(float &input, float target, float step) {
    if (input > target) {
        input -= step;
        if (input <= target) {
            input = target;
        }
    }
    else if (input < target) {
        input += step;
        if (input >= target) {
            input = target;
        }
    }
}

// Calculate camera rotation speeds related to RS movement
void Camera::calcXboxRotSpeeds() {
    // Reset speeds if the RS direction changes
    if (y_xbox_rot_speed * controller->RightStick_X() >= 0)
        y_xbox_rot_speed = 0;
    if (x_xbox_rot_speed * controller->RightStick_Y() >= 0)
        x_xbox_rot_speed = 0;

    // Read Xbox controller input; update follow speeds
    if (!controller->RStick_InDeadzone()) {
        x_xbox_rot_speed -= controller->RightStick_Y() * FOLLOW_X_CAM_XBOX_SPEED;
        y_xbox_rot_speed -= controller->RightStick_X() * FOLLOW_Y_CAM_XBOX_SPEED;

        clamp(x_xbox_rot_speed, FOLLOW_X_MAX_XBOX_SPEED);
        clamp(y_xbox_rot_speed, FOLLOW_Y_MAX_XBOX_SPEED);
    }
    else {
        converge(x_xbox_rot_speed, 0.0f, 0.2); // TODO: add a param for this step?
        converge(y_xbox_rot_speed, 0.0f, 0.2);
    }

    RS_Y = (y_xbox_rot_speed <= DELTA && y_xbox_rot_speed >= -DELTA) ? false : true;
    RS_X = (x_xbox_rot_speed <= DELTA && x_xbox_rot_speed >= -DELTA) ? false : true;
}

// Calculate camera rotation speeds related to car rotation
void Camera::calcCarRotSpeeds() {
    // Change follow speeds based on car rotation
    glm::quat diff = glm::rotate(car->getQRot() * glm::inverse(p_rot), 0.0f, up);
    float delta = 0.001;
    if (diff.y >= delta || diff.y <= -delta) { // Appreciable rotation; update rotation speeds
        y_car_rot_speed += diff.y * FOLLOW_Y_CAM_ROT_SPEED;
        clamp(y_car_rot_speed, FOLLOW_Y_MAX_ROT_SPEED);
        y_resetting = false;
    } else { // No real rotation
        if (!y_resetting) { // Just stopped rotating car; reset cam-rot speed to 0
            y_resetting = true;
            y_car_rot_speed = 0; // reset cam speed
        }

        // Slowly return to re-center on car...
        if (!RS_Y) { // ...if no RS-induced camera speed
            if (y_cam_rot <= SNAP_Y && y_cam_rot >= -SNAP_Y) { // TODO: Find a suitable general value
                y_cam_rot = 0;
                y_car_rot_speed = 0;
            }
            else if (y_cam_rot < 0) {
                y_car_rot_speed += FOLLOW_Y_CAM_RETURN_SPEED;
            }
            else if (y_cam_rot > 0) {
                y_car_rot_speed -= FOLLOW_Y_CAM_RETURN_SPEED;
            }
        }
        else {
            y_car_rot_speed = 0;
        }
        clamp(y_car_rot_speed, FOLLOW_Y_MAX_ROT_SPEED);
    }

    // MAX: Potentially clamp the sum of the individual speeds, too
    //std::cout << "X rot cam speed: " << x_rot_speed << "  X xbox cam speed: " << x_xbox_speed << "  Y rot cam speed: " <<  y_rot_speed << "  Y  xboxcam speed: " << y_xbox_speed << std::endl;

    if (diff.x >= delta || diff.x <= -delta) { // Appreciable rotation; update rotation speeds
        x_car_rot_speed += diff.x * FOLLOW_X_CAM_ROT_SPEED;
        clamp(x_car_rot_speed, FOLLOW_X_MAX_ROT_SPEED);
        x_resetting = false;
    } else { // No real rotation
        if (!x_resetting) { // Just stopped rotating car; reset cam-rot speed to 0
            x_resetting = true;
            x_car_rot_speed = 0; // reset cam speed
        }

        // Slowly return to re-center on car...
        if (!RS_X) { // ...if no RS-induced camera speed
            if (x_cam_rot <= SNAP_X && x_cam_rot >= -SNAP_X) {
                x_cam_rot = 0;
                x_car_rot_speed = 0;
            }
            else if (x_cam_rot < 0) {
                x_car_rot_speed += FOLLOW_X_CAM_RETURN_SPEED;
            }
            else if (x_cam_rot > 0) {
                x_car_rot_speed -= FOLLOW_X_CAM_RETURN_SPEED;
            }
        }
        else {
            x_car_rot_speed = 0;
        }
        clamp(x_car_rot_speed, FOLLOW_X_MAX_ROT_SPEED);
    }
}

// Reduce camera speeds as they approach the max allowed x/y angle, or 0 if RS is released
void Camera::calcAsympSpeeds() {
    if (RS_X) {
        soft_clamp_speed(x_cam_rot, x_rot_speed, FOLLOW_X_MAX_ROT, -FOLLOW_X_MAX_ROT, X_ASYMP_FACTOR);
    } else {
        if (x_rot_speed > 0) {
            soft_clamp_speed(x_cam_rot, x_rot_speed, FOLLOW_X_MAX_ROT, 0, X_ASYMP_RET_FACTOR);
        } else {
            soft_clamp_speed(x_cam_rot, x_rot_speed, 0, -FOLLOW_X_MAX_ROT, X_ASYMP_RET_FACTOR);
        }
    }

    if (RS_Y) {
        soft_clamp_speed(y_cam_rot, y_rot_speed, FOLLOW_Y_MAX_ROT, -FOLLOW_Y_MAX_ROT, Y_ASYMP_FACTOR);
    } else {
        if (y_rot_speed > 0) {
            soft_clamp_speed(y_cam_rot, y_rot_speed, FOLLOW_Y_MAX_ROT, 0, Y_ASYMP_RET_FACTOR);
        } else {
            soft_clamp_speed(y_cam_rot, y_rot_speed, 0, -FOLLOW_Y_MAX_ROT, Y_ASYMP_RET_FACTOR);
        }
    }
}


void Camera::calcFollowSpeeds() {
    calcXboxRotSpeeds();
    calcCarRotSpeeds();


    // Update camera rotation angles
    x_rot_speed = (x_car_rot_speed + x_xbox_rot_speed);
    y_rot_speed = (y_car_rot_speed + y_xbox_rot_speed);

    std::cout << "y_car_rot_speed = " << y_car_rot_speed << "    y_xbox_rot_speed" << y_xbox_rot_speed << "     y_rot_speed = " << y_rot_speed << std::endl;

    calcAsympSpeeds();

    x_cam_rot += x_rot_speed;
    clamp(x_cam_rot, FOLLOW_X_MAX_ROT);

    y_cam_rot += y_rot_speed;
    clamp(y_cam_rot, FOLLOW_Y_MAX_ROT);
}