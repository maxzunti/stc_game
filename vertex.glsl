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

out vec3 FragNormal;
out vec2 FragUV;
out vec3 lightDir;
out vec3 vecPos;

uniform mat4 cameraMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 modelviewMatrix = mat4(1.f);
// output to be interpolated between vertices and passed to the fragment stage

void main()
{
	FragNormal = normalize(
					(modelviewMatrix*vec4(VertexNormal, 0.f)).xyz
				);

	FragUV = UV;

	gl_Position = perspectiveMatrix*cameraMatrix*modelviewMatrix*vec4(VertexPosition, 1.0);
	lightDir = normalize( vec3(0,0,0) - VertexPosition);
	vecPos = VertexPosition;
}
