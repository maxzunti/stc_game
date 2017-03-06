#ifndef CAMERA_H
#define CAMERA_H
#include "../entity/Car.h"
#include "../input/input.h"
#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "../util/ConfigParser.h"


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
    enum camMode { FREE = 0, FOLLOW };
    camMode mode = FOLLOW;

    // Camera tweaking parameters:
    // Freelook variables
    float FREE_X_CAM_ROT_SPEED = 0.05;
    float FREE_Y_CAM_ROT_SPEED = 0.05;
    float FREE_X_CAM_MOVE_SPEED = 2.;
    float FREE_Y_CAM_MOVE_SPEED = 2.;
    float FREE_Z_CAM_MOVE_SPEED = 2.;

    // Follow cam variables
    fp_vars follow_vars;

    float FOLLOW_X_CAM_ROT_SPEED = 0.03;
    float FOLLOW_Y_CAM_ROT_SPEED = 0.05;
    float FOLLOW_X_CAM_XBOX_SPEED = 0.003;
    float FOLLOW_Y_CAM_XBOX_SPEED = 0.003;

    // max allowed (+/-) rotation angle, in radians
    float FOLLOW_X_MAX_ROT = 0.3;
    float FOLLOW_Y_MAX_ROT = 0.5;

    // small threshold values which determine when we automatically set the cam speeds and rot to 0
    float SNAP_X = 0.003;
    float SNAP_Y = 0.01;

    // max allowed (+/-) rotation speeds
    float FOLLOW_X_MAX_ROT_SPEED = 1.0;
    float FOLLOW_Y_MAX_ROT_SPEED = 1.0;
    float FOLLOW_X_MAX_XBOX_SPEED = 1.0;
    float FOLLOW_Y_MAX_XBOX_SPEED = 1.0;

    // Speeds to "return" to the normal orientation
    float FOLLOW_X_CAM_RETURN_SPEED = 0.0005;
    float FOLLOW_Y_CAM_RETURN_SPEED = 0.001;

    // Scaling factors which determine the cam-speeds near the angle boundaries
    float X_ASYMP_FACTOR = 3.0;
    float Y_ASYMP_FACTOR = 3.0;
    float X_ASYMP_RET_FACTOR = 0.5;
    float Y_ASYMP_RET_FACTOR = 0.5;

    float FOLLOW_DISTANCE = 6.0;
    float FOLLOW_HEIGHT = 2.0;
    float BASE_ANGLE = -0.2;
    float DELTA = 0.001;

    // Frame counts for the follow-cam delay
    const static int FOLLOW_DELAY_POS = 2;
    const static int FOLLOW_DELAY_ROT = 1;
    const static int FOLLOW_DELAY_SIZE = 16;

    glm::vec3 dir;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 pos;
	float zoom;
	
	//Camera();
	Camera(glm::vec3 _dir, glm::vec3 _pos);

    void registerController(Input *);
    void registerCar(Car *);

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
    glm::mat4 calcPerspective(); // calculate perspective as a function of car speed - want to increase FOV

    void setDims(int width, int height);
    float getCarSpeed();
    Car * getCar();
private:
    Car * car;
    Input * controller;
    ConfigParser fc_parser;

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
    glm::quat p_diff;

    int frame_counter = 0;
    int width = 0;
    int height = 0;
    
    // tracks whether the right stick is currently in use
    bool RS_Y = false;
    bool RS_X = false;
};

#endif