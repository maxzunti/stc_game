#ifndef GL_UTIL_H
#define GL_UTIL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

// Specify that we want the OpenGL core profile before including GLFW headers
#ifndef LAB_LINUX
#include <glad/glad.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

//Structs are simply acting as namespaces
//Access the values like so: VAO::LINES
struct VAO {
	enum { GEOMETRY = 0, COUNT };		//Enumeration assigns each name a value going up (LINES=0, COUNT=1)
										//
};

struct VBO {
	enum { POINTS = 0, NORMALS, UVS, INDICES, COUNT };	//POINTS=0, COLOR=1, UVS=2, INDICES=3, COUNT=4
};

struct SHADER {
	enum { DEFAULT = 0, COUNT };		// DEFAULT = 0, COUNT=1
};

// Struct to store the raw data (and vbo array, once initialized) of a loaded model
struct model_data {
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<unsigned int> indices;

    // Stores internal OpenGL pointers to each section of GPU vertex memory
    GLuint vbo[VBO::COUNT];
    GLuint vao[VAO::COUNT];

    model_data(std::vector<glm::vec3> points,
               std::vector<glm::vec2> uvs,
               std::vector<glm::vec3> normals,
               std::vector<unsigned int> indices);
};

//GLuint vbo[];		//Array which stores OpenGL's vertex buffer object handles
GLuint vao[];		//Array which stores Vertex Array Object handles
GLuint shader[];		//Array which stores shader program handles

//Gets handles from OpenGL
void generateIDs();

//Clean up IDs when you're done using them
void deleteIDs();

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

void initGL();

// OpenGL utility functions
void QueryGLVersion();
bool CheckGLErrors(std::string location);

// OpenGL shader support functions
// reads a text file with the given name into a string
std::string LoadSource(const std::string &filename);

//Compile and link shaders, storing the program ID in shader array
bool initShader();

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const std::string &source);

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);


#endif // GL_UTIL_H