// Silhouette fragment shader
// Inspired by http://sunandblackcat.com/tipFullView.php?l=eng&topicid=15
// (We ultimately use a totally different technique, but this link served as a basis)
// (Fragment shader used verbatim - simple flat-color shading)
#version 330

uniform vec3 u_color1;

layout(location = 0) out vec4 o_FragColor;

void main(void){
   o_FragColor = vec4(u_color1, 1.0);
}