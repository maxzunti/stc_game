#include "camera.h"
#include <cstdio>

#include <iostream>
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

Camera::Camera():	dir(vec3(0, 0, -1)), 
					right(vec3(1, 0, 0)), 
					up(vec3(0, 1, 0)),
					pos(vec3(0, 0, 0)),
					zoom(0.f)
{}

Camera::Camera(vec3 _dir, vec3 _pos):dir(normalize(_dir)), pos(_pos)
{
	right = normalize(cross(_dir, vec3(0, 1, 0)));
	up =  normalize(cross(right, _dir));
	zoom = -4.0f;
}

/*
	[ Right 0 ]
	[ Up 	0 ]
	[ -Dir	0 ]
	[ 0 0 0 1 ]
*/

mat4 Camera::getMatrix()
{
	mat4 cameraRotation = mat4(
			vec4(right, 0),
			vec4(up, 0),
			vec4(-dir, 0),
			vec4(0, 0, 0, 1));

	mat4 translation = mat4 (
			vec4(1, 0, 0, 0),
			vec4(0, 1, 0, 0),
			vec4(0, 0, 1, 0),
			vec4(-pos, 1));

	return transpose(cameraRotation)*translation;
}

void Camera::rotateAroundCenter(float x, float y, vec3 focus)
{
	vec3 camFocus = pos - focus;
	mat4 rotateAroundUp = rotateAbout(vec3(0,1,0), (zoom <= -1)?x*abs(zoom):x);
	mat4 rotateAroundRight = rotateAbout(right, (zoom <= -1)?y*abs(zoom):y);
	
	vec3 posRelOrigin =  rotateAroundUp * rotateAroundRight * vec4(camFocus,0);
	posRelOrigin += camFocus;
	
	vec3 normalized = normalize(posRelOrigin);
	(normalized.y > 0.65)? normalized.y = 0.65:normalized.y;
	(normalized.y < -0.65)? normalized.y = -0.65:normalized.y;

	pos = normalized + dir*zoom;
	dir = -1.f*normalize(camFocus);
	right = normalize(cross(dir, vec3(0,1,0)));
	up = normalize(cross(right, dir));
	
}


void Camera::rotateCamera(float x, float y)
{
	mat4 rotateAroundY = rotateAbout(vec3(0, 1, 0), x);
	mat4 rotateAroundX = rotateAbout(right, y);

	dir = normalize(rotateAroundX*rotateAroundY*vec4(dir, 0));
	right = normalize(cross(dir, vec3(0, 1, 0)));
	up =  normalize(cross(right, dir));
}
