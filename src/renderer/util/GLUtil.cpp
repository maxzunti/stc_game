#include "GLUtil.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Not sure if all of these are necessary - potentially trim
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace glm;
#define PI 3.14159265359

GLuint vbo[VBO::COUNT];		//Array which stores OpenGL's vertex buffer object handles
GLuint vao[VAO::COUNT];		//Array which stores Vertex Array Object handles
GLuint shader[SHADER::COUNT];		//Array which stores shader program handles

// Specify that we want the OpenGL core profile before including GLFW headers
#ifndef LAB_LINUX
#include <glad/glad.h>
#else
#define GLFW_INCLUDE_GLCOREARB
#define GL_GLEXT_PROTOTYPES
#endif
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

model_data::model_data(std::vector<glm::vec3> points,
                       std::vector<glm::vec2> uvs,
                       std::vector<glm::vec3> normals,
                       std::vector<unsigned int> indices)
{
    glGenBuffers(VBO::COUNT, vbo);
    this->points = points;
    this->normals = normals;
    this->uvs = uvs;
    this->indices = indices;
    initVAO(vbo);
    loadBuffer(vbo, points, normals, uvs, indices);
}

//Gets handles from OpenGL
void generateIDs()
{
	glGenVertexArrays(VAO::COUNT, vao);		//Tells OpenGL to create VAO::COUNT many
											// Vertex Array Objects, and store their
											// handles in vao array
	glGenBuffers(VBO::COUNT, vbo);		//Tells OpenGL to create VBO::COUNT many
										//Vertex Buffer Objects and store their
										//handles in vbo array
}

//Clean up IDs when you're done using them
void deleteIDs()
{
	for (int i = 0; i<SHADER::COUNT; i++)
	{
		glDeleteProgram(shader[i]);
	}

	glDeleteVertexArrays(VAO::COUNT, vao);
	glDeleteBuffers(VBO::COUNT, vbo);
}

//Describe the setup of the Vertex Array Object
bool initVAO(GLuint vbo[VBO::COUNT]) // MAX: I think I need to pass a new 'vbo' object (w/ its own pointers) and call this multiple times
{
	//glBindVertexArray(vao[VAO::GEOMETRY]);		//Set the active Vertex Array (should only have 1, right?)

	glEnableVertexAttribArray(0);		//Tell opengl you're using layout attribute 0 (For shader input)
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::POINTS]);		//Set the active Vertex Buffer
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_FALSE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glVertexAttribPointer(
		1,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_FALSE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	glEnableVertexAttribArray(2);		//Tell opengl you're using layout attribute 1
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glVertexAttribPointer(
		2,
		2,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vec2),
		(void*)0
		);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::INDICES]);

	return !CheckGLErrors("initVAO");		//Check for errors in initialize
}


//Loads buffers with data
bool loadBuffer(GLuint vbo[VBO::COUNT],
                const vector<vec3>& points,
                const vector<vec3> normals,
                const vector<vec2>& uvs,
                const vector<unsigned int>& indices)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::POINTS]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*points.size(),	//Size of data in array (in bytes)
		&points[0],							//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW						//GL_DYNAMIC_DRAW if you're changing the data often
											//GL_STATIC_DRAW if you're changing seldomly
		);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glBufferData(
		GL_ARRAY_BUFFER,				//Which buffer you're loading too
		sizeof(vec3)*normals.size(),	//Size of data in array (in bytes)
		&normals[0],							//Start of array (&points[0] will give you pointer to start of vector)
		GL_STATIC_DRAW						//GL_DYNAMIC_DRAW if you're changing the data often
											//GL_STATIC_DRAW if you're changing seldomly
		);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vec2)*uvs.size(),
		&uvs[0],
		GL_STATIC_DRAW
		);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::INDICES]);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)*indices.size(),
		&indices[0],
		GL_STATIC_DRAW
		);

	return !CheckGLErrors("loadBuffer");
}

//Compile and link shaders, storing the program ID in shader array
bool initShader()
{
	string vertexSource = LoadSource("src\\renderer\\shaders\\vertex.glsl");		//Put vertex file text into string
	string fragmentSource = LoadSource("src\\renderer\\shaders\\fragment.glsl");		//Put fragment file text into string

	GLuint vertexID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	shader[SHADER::DEFAULT] = LinkProgram(vertexID, fragmentID);	//Link and store program ID in shader array

	return !CheckGLErrors("initShader");
}

//For reference:
//	https://open.gl/textures
GLuint createTexture(const char* filename)
{
	int components;
	GLuint texID;
	int tWidth, tHeight;

	//stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename, &tWidth, &tHeight, &components, 0);

	if (data != NULL)
	{
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);

		if (components == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tWidth, tHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (components == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tWidth, tHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//Clean up
		glBindTexture(GL_TEXTURE_2D, 0);
		stbi_image_free(data);

		return texID;
	}

	return 0;	//Error
}

//Use program before loading texture
//	texUnit can be - GL_TEXTURE0, GL_TEXTURE1, etc...
bool loadTexture(GLuint texID, GLuint texUnit, GLuint program, const char* uniformName)
{
	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, texID);

	GLuint uniformLocation = glGetUniformLocation(program, uniformName);
	glUniform1i(uniformLocation, 0);

	return !CheckGLErrors("loadTexture");
}


//Initialization
void initGL()
{
	//COMMENT/UNCOMMENT - essentially turns wire frames on and off
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//Only call these once - don't call again every time you change geometry
	//generateIDs();		//Create VertexArrayObjects and Vertex Buffer Objects and store their handles

    glGenVertexArrays(VAO::COUNT, vao);		//Tells OpenGL to create VAO::COUNT many
                                            // Vertex Array Objects, and store their
                                            // handles in vao array

	initShader();		//Create shader and store program ID

	//initVAO();			//Describe setup of Vertex Array Objects and Vertex Buffer Object
    glBindVertexArray(vao[VAO::GEOMETRY]);		//Set the active Vertex Array (should only have 1, right?)


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	/*
	for (int i = 0; i < sizeof(*imageNames); i++) {
	textureIds[i] = createTexture(imageNames[i]);
	}*/
}

// ==========================================================================
// SUPPORT FUNCTION DEFINITIONS

// --------------------------------------------------------------------------
// OpenGL utility functions

void QueryGLVersion()
{
	// query opengl version and renderer information
	string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

	cout << "OpenGL [ " << version << " ] "
		<< "with GLSL [ " << glslver << " ] "
		<< "on renderer [ " << renderer << " ]" << endl;
}

bool CheckGLErrors(string location)
{
	bool error = false;
	for (GLenum flag = glGetError(); flag != GL_NO_ERROR; flag = glGetError())
	{
		cout << "OpenGL ERROR:  ";
		switch (flag) {
		case GL_INVALID_ENUM:
			cout << location << ": " << "GL_INVALID_ENUM" << endl; break;
		case GL_INVALID_VALUE:
			cout << location << ": " << "GL_INVALID_VALUE" << endl; break;
		case GL_INVALID_OPERATION:
			cout << location << ": " << "GL_INVALID_OPERATION" << endl; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			cout << location << ": " << "GL_INVALID_FRAMEBUFFER_OPERATION" << endl; break;
		case GL_OUT_OF_MEMORY:
			cout << location << ": " << "GL_OUT_OF_MEMORY" << endl; break;
		default:
			cout << "[unknown error code]" << endl;
		}
		error = true;
	}
	return error;
}

// --------------------------------------------------------------------------
// OpenGL shader support functions

// reads a text file with the given name into a string
string LoadSource(const string &filename)
{
	string source;

	ifstream input(filename.c_str());
	if (input) {
		copy(istreambuf_iterator<char>(input),
			istreambuf_iterator<char>(),
			back_inserter(source));
		input.close();
	}
	else {
		cout << "ERROR: Could not load shader source from file "
			<< filename << endl;
	}

	return source;
}

// creates and returns a shader object compiled from the given source
GLuint CompileShader(GLenum shaderType, const string &source)
{
	// allocate shader object name
	GLuint shaderObject = glCreateShader(shaderType);

	// try compiling the source as a shader of the given type
	const GLchar *source_ptr = source.c_str();
	glShaderSource(shaderObject, 1, &source_ptr, 0);
	glCompileShader(shaderObject);

	// retrieve compile status
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetShaderInfoLog(shaderObject, info.length(), &length, &info[0]);
		cout << "ERROR compiling shader:" << endl << endl;
		cout << source << endl;
		cout << info << endl;
	}

	return shaderObject;
}

// creates and returns a program object linked from vertex and fragment shaders
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
	// allocate program object name
	GLuint programObject = glCreateProgram();

	// attach provided shader objects to this program
	if (vertexShader)   glAttachShader(programObject, vertexShader);
	if (fragmentShader) glAttachShader(programObject, fragmentShader);

	// try linking the program with given attachments
	glLinkProgram(programObject);

	// retrieve link status
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		string info(length, ' ');
		glGetProgramInfoLog(programObject, info.length(), &length, &info[0]);
		cout << "ERROR linking shader program:" << endl;
		cout << info << endl;
	}

	return programObject;
}


// ==========================================================================