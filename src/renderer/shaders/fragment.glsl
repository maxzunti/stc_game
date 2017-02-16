// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

uniform sampler2D image;
// interpolated colour received from vertex stage

uniform int shade;

// first output is mapped to the framebuffer's colour index by default
out vec4 FragmentColour;

in vec3 FragNormal;
in vec2 FragUV;
in vec3 lightDir;
in vec3 vecPos;
void main(void)
{

	float ka = 0.7f;
	float kd = 0.5f;
	float ks = 0.2f;
	
	float intensity = 1.f;
	
	//Maps the texture onto the sphere
    vec3 n_normal = normalize(FragNormal);
	vec3 s = normalize(lightDir);
	vec3 v = normalize(-vecPos);
	vec3 r = reflect( -s,n_normal);

    vec2 texture_coordinate;
    texture_coordinate.x = FragUV[0];
	texture_coordinate.y = -FragUV[1];
	
	vec3 h = reflect(-lightDir, FragNormal);
	
	//FragmentColour = vec4(normalize(FragNormal), 1);
    FragmentColour = texture(image, texture_coordinate);

	//FragmentColour = texture(image, texture_coordinate);
	//FragmentColour = vec4(normalize(FragNormal), 1);
		
	vec4 ambient = intensity * ka * FragmentColour;
	
	float sDotN = max( dot( s, n_normal ), 0.0 );
	vec4 diffuse = intensity * kd * sDotN * FragmentColour;
	
	// These guys really aren't that shiny so we'll give em a low exponent
	vec4 specular = intensity * ks * pow(max(dot(n_normal,v), 0.0), 1.0f)*FragmentColour;
	
	FragmentColour = ambient + diffuse + specular;

}
