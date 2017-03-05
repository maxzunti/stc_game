// Silhouette vertex shader
// Inspired by http://sunandblackcat.com/tipFullView.php?l=eng&topicid=15
// (We ultimately use a totally different technique, but this link served as a basis)
#version 330

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

uniform mat4 cameraMatrix;
uniform mat4 perspectiveMatrix;
uniform mat4 modelviewMatrix;

void main(void){
   vec4 tPos   = vec4(VertexPosition, 1.0);
   gl_Position = perspectiveMatrix * cameraMatrix * modelviewMatrix * tPos;
}