// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

// location indices for these attributes correspond to those specified in the
// InitializeGeometry() function of the main program
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in vec2 UV;

out vec3 vecPos;
out vec3 FragNormal;
out vec2 FragUV;

out vec4 ShadowCoord;
uniform mat4 depthBiasMVP;

uniform mat4 cameraMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 modelviewMatrix;
// output to be interpolated between vertices and passed to the fragment stage

void main()
{
	gl_Position = perspectiveMatrix*cameraMatrix*modelviewMatrix*vec4(VertexPosition, 1.0);
	//vecPos = VertexPosition;
	vecPos = vec3(modelviewMatrix*vec4(VertexPosition,1.f));

	//FragNormal = normalize((modelviewMatrix*vec4(VertexNormal, 0.f)).xyz);

	FragNormal = transpose(inverse(mat3(modelviewMatrix))) * VertexNormal;

	FragUV = UV;
	ShadowCoord = depthBiasMVP * modelviewMatrix * vec4(VertexPosition,1);
}
