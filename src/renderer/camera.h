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
    ProtoCar * car;
    Input * controller;

    void setMode();
    void freeLook();
    void followLook();

public:
    const enum camMode { FREE = 0, FOLLOW };
    camMode mode = FOLLOW;

    // Camera tweaking parameters:
    // Freelook
    const float FREE_X_CAM_ROT_SPEED = 0.05;
    const float FREE_Y_CAM_ROT_SPEED = 0.05;
    const float FREE_X_CAM_MOVE_SPEED = 0.2;
    const float FREE_Y_CAM_MOVE_SPEED = 0.2;
    const float FREE_Z_CAM_MOVE_SPEED = 0.2;

    // Follow cam
    const float FOLLOW_X_CAM_ROT_SPEED = 0.4;
    const float FOLLOW_Y_CAM_ROT_SPEED = 0.2;

    const float FOLLOW_DISTANCE = 8.0;
    const float FOLLOW_HEIGHT = 2.0;
    const float BASE_ANGLE = -0.2;

  /*  const int FOLLOW_DELAY_POS = 8;
    const int FOLLOW_DELAY_ROT = 8;
    glm::vec3 prev_pos[8];
    glm::quat prev_rot[8]; */

    glm::vec3 dir;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 pos;
	float zoom;
	
	Camera();
	Camera(glm::vec3 _dir, glm::vec3 _pos);

    void registerController(Input *);
    void registerCar(ProtoCar *);

    void update();
	void rotateCamera(float x, float y);
	void rotateAroundCenter(float x, float y, glm::vec3 focus);
    void movePosition(glm::vec3 delta);
    void quatRot(glm::tquat<double> q);
    glm::mat4 getMatrix();

};

#endif