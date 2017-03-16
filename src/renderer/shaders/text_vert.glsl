#version 330 core

// Adapted/Inspired by http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-11-2d-text/
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 vertexPosition_screenspace;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

uniform int height;
uniform int width;

void main(){

	// Output position of the vertex, in clip space
	// This is related to screen size. Needs to be changed according to the screen size
	// map screensize to [-1..1][-1..1] 

	vec2 vertexPosition_homogeneousspace = vertexPosition_screenspace - vec2(width/2,height/2);
	vertexPosition_homogeneousspace /= vec2(width/2,height/2);
	gl_Position =  vec4(vertexPosition_homogeneousspace,0,1);
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}