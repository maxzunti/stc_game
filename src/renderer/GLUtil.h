#ifndef GL_UTIL_H
#define GL_UTIL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

#include <stb_image.h>

//Structs are simply acting as namespaces
//Access the values like so: VAO::LINES
struct VAO {
	enum { GEOMETRY = 0, COUNT };		//Enumeration assigns each name a value going up (LINES=0, COUNT=1)
};

struct VBO {
	enum { POINTS = 0, NORMALS, UVS, INDICES, COUNT };	//POINTS=0, COLOR=1, UVS=2, INDICES=3, COUNT=4
};

enum SHADER { DEFAULT = 0, SKYBOX, SHADER_COUNT };

//GLuint vbo[];		//Array which stores OpenGL's vertex buffer object handles
GLuint vao[];		//Array which stores Vertex Array Object handles
GLuint shader[];		//Array which stores shader program handles

//Gets handles from OpenGL
void generateIDs();

//Clean up IDs when you're done using them
void deleteIDs(GLuint vao[], GLuint vbo[]);

//Describe the setup of the Vertex Array Object
bool initVAO(GLuint vao[], GLuint vbo[]);

//Loads buffers with data
bool loadBuffer(GLuint[], const std::vector<glm::vec3>& points, const std::vector<glm::vec3> normals,
	const std::vector<glm::vec2>& uvs, const std::vector<unsigned int>& indices);

//For reference:
//	https://open.gl/textures
GLuint createTexture(const char* filename);

//Use program before loading texture
//	texUnit can be - GL_TEXTURE0, GL_TEXTURE1, etc...
bool loadTexture(GLuint texID, GLuint texUnit, GLuint program, const char* uniformName);

bool initGL();

// OpenGL utility functions
void QueryGLVersion();
bool CheckGLErrors(std::string location);

// OpenGL shader support functions
// reads a text file with the given name into a string
std::string LoadSource(const std::string &filename);

//Compile and link shaders, storing the program ID in shader array
bool initShader(SHADER shad, std::string vs, std::string fs);

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const std::string &source);

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);


#endif // GL_UTIL_H