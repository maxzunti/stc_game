#ifndef CAMERA_H
#define CAMERA_H
#include "../entity/ProtoCar.h"
#include "../input/input.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"


/*
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
*/

class Camera{
public:
    const enum camMode { FREE = 0, FOLLOW };
    camMode mode = FOLLOW;

    // Camera tweaking parameters:
    // Freelook variables
    const float FREE_X_CAM_ROT_SPEED = 0.05;
    const float FREE_Y_CAM_ROT_SPEED = 0.05;
    const float FREE_X_CAM_MOVE_SPEED = 0.2;
    const float FREE_Y_CAM_MOVE_SPEED = 0.2;
    const float FREE_Z_CAM_MOVE_SPEED = 0.2;

    // Follow cam variables
    const float FOLLOW_X_CAM_ROT_SPEED = 0.03;
    const float FOLLOW_Y_CAM_ROT_SPEED = 0.05;
    const float FOLLOW_X_CAM_XBOX_SPEED = 0.003;
    const float FOLLOW_Y_CAM_XBOX_SPEED = 0.003;

    // max allowed (+/-) rotation angle, in radians
    const float FOLLOW_X_MAX_ROT = 0.3;
    const float FOLLOW_Y_MAX_ROT = 0.5;

    // small threshold values which determine when we automatically set the cam speeds and rot to 0
    const float SNAP_X = 0.003;
    const float SNAP_Y = 0.01;

    // max allowed (+/-) rotation speeds
    const float FOLLOW_X_MAX_ROT_SPEED = 1.0;
    const float FOLLOW_Y_MAX_ROT_SPEED = 1.0;
    const float FOLLOW_X_MAX_XBOX_SPEED = 1.0;
    const float FOLLOW_Y_MAX_XBOX_SPEED = 1.0;

    // Speeds to "return" to the normal orientation
    const float FOLLOW_X_CAM_RETURN_SPEED = 0.0005;
    const float FOLLOW_Y_CAM_RETURN_SPEED = 0.001;

    // Scaling factors which determine the cam-speeds near the angle boundaries
    const float X_ASYMP_FACTOR = 3.0;
    const float Y_ASYMP_FACTOR = 3.0;
    const float X_ASYMP_RET_FACTOR = 0.5;
    const float Y_ASYMP_RET_FACTOR = 0.5;

    const float FOLLOW_DISTANCE = 6.0;
    const float FOLLOW_HEIGHT = 2.0;
    const float BASE_ANGLE = -0.2;
    const float DELTA = 0.001;

    // Frame counts for the follow-cam delay
    static const int FOLLOW_DELAY_POS = 3;
    static const int FOLLOW_DELAY_ROT = 4;
    static const int FOLLOW_DELAY_SIZE = 16;

    glm::vec3 dir;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 pos;
	float zoom;
	
	//Camera();
	Camera(glm::vec3 _dir, glm::vec3 _pos);

    void registerController(Input *);
    void registerCar(ProtoCar *);

    void update();
	void rotateCamera(float x, float y);
	void rotateAroundCenter(float x, float y, glm::vec3 focus);
    void movePosition(glm::vec3 delta);
    void quatRot(glm::tquat<double> q);

    // Functions for calculating follow cam speeds and position
    void calcFollowSpeeds();
    void calcXboxRotSpeeds();
    void calcCarRotSpeeds();
    void calcAsympSpeeds(); // asymptotically lower rotation speed as the camera returns to 0 or reaches its boundaries
    void clamp(float &input, float abs_max);
    void soft_clamp_speed(const float &curr, float &speed, float max, float min, float factor); // asymptotically lower speeds towards certain thresholds
    void converge(float &input, float target, float step);

    glm::mat4 getMatrix();

private:
    ProtoCar * car;
    Input * controller;

    float x_cam_rot = 0;
    float y_cam_rot = 0;

    // Follow-cam speeds induced by RS movement
    float x_xbox_rot_speed = 0;
    float y_xbox_rot_speed = 0;

    // Follow-cam speeds induced by car rotation
    float x_car_rot_speed = 0;
    float y_car_rot_speed = 0;

    // Total x/y follow-cam speeds
    float x_rot_speed;
    float y_rot_speed;

    bool y_resetting = false;
    bool x_resetting = false;

    void setMode();
    void freeLook();
    void followLook();

    glm::vec3 prev_pos[FOLLOW_DELAY_SIZE];
    glm::quat prev_rot[FOLLOW_DELAY_SIZE];
    glm::quat p_rot;

    int frame_counter = 0;
    
    // tracks whether the right stick is currently in use
    bool RS_Y = false;
    bool RS_X = false;
};

#endif