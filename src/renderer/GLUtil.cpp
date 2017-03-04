#include "GLUtil.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace glm;
#define PI 3.14159265359
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint shader[SHADER::SHADER_COUNT];		//Array which stores shader program handles

//Clean up IDs when you're done using them
/*
TODO: replicate me within Renderer/Model classes as appropriate
void deleteIDs()
{
	for (int i = 0; i<SHADER::COUNT; i++)
	{
		glDeleteProgram(shader[i]);
	}

	glDeleteVertexArrays(VAO::COUNT, vao);
	glDeleteBuffers(VBO::COUNT, vbo);
}
*/

//Describe the setup of the Vertex Array Object
bool initVAO(GLuint vao[VAO::COUNT], GLuint vbo[VBO::COUNT])
{
	glBindVertexArray(vao[VAO::GEOMETRY]);		//Set the active Vertex Array (should only have 1, right?)

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

	glEnableVertexAttribArray(1);       //Tell opengl you're using layout attribute 1
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glVertexAttribPointer(
		1,				//Attribute
		3,				//Size # Components
		GL_FLOAT,	//Type
		GL_FALSE, 	//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	glEnableVertexAttribArray(2);       //Tell opengl you're using layout attribute 2
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

    if (uvs.size() > 0) { // eww.
        glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vec2)*uvs.size(),
            &uvs[0],
            GL_STATIC_DRAW
            );
    }

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
bool initShader(SHADER shad, std::string vs, std::string fs)
{
	string vertexSource = LoadSource(vs);		//Put vertex file text into string
	string fragmentSource = LoadSource(fs);		//Put fragment file text into string

	GLuint vertexID = CompileShader(GL_VERTEX_SHADER, vertexSource);
	GLuint fragmentID = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	shader[shad] = LinkProgram(vertexID, fragmentID);	//Link and store program ID in shader array

	return !CheckGLErrors("initShader");
}


//Initialization
bool initGL()
{
    //Intialize GLAD
    if (!gladLoadGL())
    {
        cout << "GLAD init failed" << endl;
        return false;
    }

    std::cout << GLVersion.major << "   " << GLVersion.minor << std::endl;


    // query and print out information about our OpenGL environment
    QueryGLVersion();

	//COMMENT/UNCOMMENT - essentially turns wire frames on and off
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    initShader(SHADER::DEFAULT, "src\\renderer\\shaders\\vertex.glsl", "src\\renderer\\shaders\\fragment.glsl");
    initShader(SHADER::SKYBOX, "src\\renderer\\shaders\\skybox_vert.glsl", "src\\renderer\\shaders\\skybox_frag.glsl");
    initShader(SHADER::SHADOW, "src\\renderer\\shaders\\shadow_vert.glsl", "src\\renderer\\shaders\\shadow_frag.glsl");
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	/*
	for (int i = 0; i < sizeof(*imageNames); i++) {
	textureIds[i] = createTexture(imageNames[i]);
	}*/
    return true;
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