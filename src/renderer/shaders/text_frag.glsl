#version 330 core


// Adapted/Inspired by http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-11-2d-text/
// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D textTextureSample;
uniform float alphaMod;

void main(){

	color = texture( textTextureSample, UV );
	color.a *= alphaMod;
}