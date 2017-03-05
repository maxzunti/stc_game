#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec2 vertexPosition_screenspace;
layout(location = 1) in vec2 vertexUV;

// Output data ; will be interpolated for each fragment.
out vec2 UV;

void main(){

	// Output position of the vertex, in clip space
	// This is related to screen size. Needs to be changed according to the screen size
	// map [0..800][0..600] to [-1..1][-1..1] 

	vec2 vertexPosition_homogeneousspace = vertexPosition_screenspace - vec2(512,384); // [0..800][0..600] -> [-400..400][-300..300]
	vertexPosition_homogeneousspace /= vec2(512,384);
	gl_Position =  vec4(vertexPosition_homogeneousspace,0,1);
	
	// UV of the vertex. No special space for this one.
	UV = vertexUV;
}

