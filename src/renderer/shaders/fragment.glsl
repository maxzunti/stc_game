// ==========================================================================
// Vertex program for barebones GLFW boilerplate
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================
#version 410

uniform sampler2D image;

// When true - renders the depth map (shadowMap texture) on every object
bool DEBUG = false;

out vec4 FragmentColour;

in vec3 vecPos;
in vec3 FragNormal;
in vec2 FragUV;
in vec3 lightDir;

// NEW STUFF//////////////////////////////////////
in vec4 ShadowCoord;

// Values that stay constant for the whole mesh.
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

// Calculate whether something is in shadow or not
// Apply some techniques that improve the the shadow quality
float ShadowCalculation(vec4 shadowCoord)
{
	
    // perform perspective divide - used for transforming from perspective to orthogrpahic.
	// coords will be in -1 to 1 range
    vec3 projCoords = ShadowCoord.xyz / ShadowCoord.w;

    // Transform from [-1,1] to [0,1] range
    projCoords = (projCoords * 0.5) + 0.5;

    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // Calculate bias (based on depth map resolution and slope) - one of the techniques to improve shadow quality
    vec3 normal = normalize(FragNormal);
    vec3 lightDir = normalize(lightPos - vecPos);
    float bias = max(0.001 * (1.0 - dot(normal, lightDir)), 0.0002); // Changing the bias has great effects on the rendering
	
    // Check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;


    // PCF - another technique to improve shadows - its use will depend on the quality of shadows we're getting
	float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;


   // Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
       shadow = 0.0;
     
    return shadow;
}

void main()
{     
	//shading variables (coefficients and phong exponent(p))
	float ka = 0.2f;
	float kd = 0.5f;
	float ks = 0.2f;
	float p = 64.0;

	float intensity = 1.f;

    vec2 texture_coordinate;
    texture_coordinate.x = FragUV[0];
	texture_coordinate.y = -FragUV[1];

	// could be xyz instead of rgb (rgb usage due to the fact that its a color)
    vec4 color =  texture(image, texture_coordinate); 

    vec3 normal = normalize(FragNormal);
    vec3 lightColor = vec3(1.0);

    // Ambient
    vec3 ambient = ka * color.rgb;
	
	// Diffuse
    vec3 lightDir = normalize(lightPos - vecPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    
	// Specular
    vec3 viewDir = normalize(viewPos - vecPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), p);
    vec3 specular = spec * lightColor;    

    // Calculate shadow
    float shadow = ShadowCalculation(ShadowCoord);                      
    shadow = min(shadow, 0.75); // reduce shadow strength a little: allow some diffuse/specular light in shadowed regions
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color.rgb;    
  
	FragmentColour = vec4(lighting, color.a);
  
	// DEBUG sets the texture on each object to be the depth map
	if (DEBUG){
		texture_coordinate.y = -texture_coordinate.y;
		float temp = texture(shadowMap, texture_coordinate).r;
		FragmentColour = vec4( vec3(temp),1.f);
	
		// This is another useful debugging option
		// Allows visualization of the depth map on each of the objects
		/*vec3 projCoords = ShadowCoord.xyz / ShadowCoord.w;
		projCoords = (projCoords * 0.5) + 0.5;
		FragmentColour = vec4(texture(shadowMap, projCoords.xy).x);
		//FragmentColour = vec4(projCoords.xy,0,0 );
		*/

	}
}
//////////////////////////////////////////////////
