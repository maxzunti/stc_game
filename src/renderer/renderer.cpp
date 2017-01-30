// ==========================================================================
// Barebones OpenGL Core Profile Boilerplate
//    using the GLFW windowing system (http://www.glfw.org)
//
// Loosely based on
//  - Chris Wellons' example (https://github.com/skeeto/opengl-demo) and
//  - Camilla Berglund's example (http://www.glfw.org/docs/latest/quick.html)
//
// Author:  Sonny Chan, University of Calgary
// Date:    December 2015
// ==========================================================================

#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <iterator>
#include <vector>
#include <cstdlib>
#include <ctime>

#include "renderer.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

#include "camera.h"
using namespace std;
using namespace glm;
#define PI 3.14159265359


//Forward definitions
bool CheckGLErrors(string location);
void QueryGLVersion();
string LoadSource(const string &filename);
GLuint CompileShader(GLenum shaderType, const string &source);
GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

char sun[] = "texture_sun.jpg";
char earth[] = "texture_earth_surface.jpg";
char moon[] = "texture_moon.jpg";
char stars[] = "stars_milkyway.jpg";

char *imageNames[] = { sun,earth,moon,stars };

GLuint textureIds[4];
int shade = 0;

vec2 mousePos;
bool mousePressed = false;

Camera cam;
vector<vec3> points;
vector<vec2> uvs;
vector<vec3> normals;
vector<unsigned int> indices;

vector<vec3> points2;
vector<vec3> normals2;
vector<vec2> uvs2;
vector<unsigned int> indices2;

vector<vec3> points3;
vector<vec3> normals3;
vector<vec2> uvs3;
vector<unsigned int> indices3;

vector<vec3> points4;
vector<vec3> normals4;
vector<vec2> uvs4;
vector<unsigned int> indices4;


vec3 earthCenter = vec3(-3.24f, 0, 0);
vec3 moonCenter = vec3(-3.24f, 0, 2.42);

GLFWwindow* window = 0;

// --------------------------------------------------------------------------
// GLFW callback functions

// reports GLFW errors
void ErrorCallback(int error, const char* description)
{
    cout << "GLFW ERROR " << error << ":" << endl;
    cout << description << endl;
}

mat4 rotateAboutAxis(vec3 axis, float radians)
{
    mat4 matrix;

    matrix[0][0] = cos(radians) + axis.x*axis.x*(1 - cos(radians));
    matrix[1][0] = axis.x*axis.y*(1 - cos(radians)) - axis.z*sin(radians);
    matrix[2][0] = axis.x*axis.z*(1 - cos(radians)) + axis.y*sin(radians);

    matrix[0][1] = axis.y*axis.x*(1 - cos(radians)) + axis.z*sin(radians);
    matrix[1][1] = cos(radians) + axis.y*axis.y*(1 - cos(radians));
    matrix[2][1] = axis.y*axis.z*(1 - cos(radians)) - axis.x*sin(radians);

    matrix[0][2] = axis.z*axis.x*(1 - cos(radians)) - axis.y*sin(radians);
    matrix[1][2] = axis.z*axis.y*(1 - cos(radians)) + axis.x*sin(radians);
    matrix[2][2] = cos(radians) + axis.z*axis.z*(1 - cos(radians));

    return matrix;
}

// handles keyboard input events
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    float move = 0.05f;

    if (key == GLFW_KEY_W) {
        if (cam.zoom < -0.5) {
            cam.pos += cam.dir*move;
            cam.zoom += move;
        }
        else {
            cam.zoom = -0.5f;
        }
    }
    else if (key == GLFW_KEY_S) {
        if (cam.zoom > -10) {
            cam.pos -= cam.dir*move;
            cam.zoom -= move;
        }
        else {
            cam.zoom = -10.0f;
        }
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if ((action == GLFW_PRESS) || (action == GLFW_RELEASE))
        mousePressed = !mousePressed;
}

void mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    vec2 newPos = vec2(xpos / (double)vp[2], -ypos / (double)vp[3])*2.f - vec2(1.f);

    vec2 diff = newPos - mousePos;
    if (mousePressed) {
        cam.rotateAroundCenter(-diff.x, diff.y, vec3(0, 0, 0));
    }
    mousePos = newPos;
}

void resizeCallback(GLFWwindow* window, int width, int height)
{
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    glViewport(0, 0, width, height);
}





//==========================================================================

//Structs are simply acting as namespaces
//Access the values like so: VAO::LINES
struct VAO {
    enum { GEOMETRY = 0, COUNT };		//Enumeration assigns each name a value going up
                                        //LINES=0, COUNT=1
};

struct VBO {
    enum { POINTS = 0, NORMALS, UVS, INDICES, COUNT };	//POINTS=0, COLOR=1, COUNT=2
};

struct SHADER {
    enum { DEFAULT = 0, COUNT };		//LINE=0, COUNT=1
};

GLuint vbo[VBO::COUNT];		//Array which stores OpenGL's vertex buffer object handles
GLuint vao[VAO::COUNT];		//Array which stores Vertex Array Object handles
GLuint shader[SHADER::COUNT];		//Array which stores shader program handles

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
bool initVAO()
{
    glBindVertexArray(vao[VAO::GEOMETRY]);		//Set the active Vertex Array

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
bool loadBuffer(const vector<vec3>& points, const vector<vec3> normals,
    const vector<vec2>& uvs, const vector<unsigned int>& indices)
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

//u parameterizes in the big circle, v parameterizes in the little circle
//c_r is the circle radius, t_r is the tube radius
void generateTorus(vector<vec3>& positions, vector<vec3>& normals,
    vector<vec2>& uvs, vector<unsigned int>& indices,
    float c_r, float t_r,
    int uDivisions, int vDivisions)
{
    float uStep = 1.f / (float)(uDivisions - 1);
    float vStep = 1.f / (float)(vDivisions - 1);

    float u = 0.f;

    //Traversing u
    for (int i = 0; i<uDivisions; i++)
    {

        vec3 center = vec3(c_r*cos(2.f*PI*u),
            0.f,
            c_r*sin(2.f*PI*u));

        float v = 0.f;

        //Traversing v
        for (int j = 0; j<vDivisions; j++)
        {
            vec3 pos = vec3((c_r + t_r*cos(2.f*PI*v)) * cos(2.f*PI*u),
                t_r*sin(2.f*PI*v),
                (c_r + t_r*cos(2.f*PI*v)) * sin(2.f*PI*u));

            vec3 normal = normalize(pos - center);

            positions.push_back(pos);
            normals.push_back(normal);
            uvs.push_back(vec2(u, v));

            v += vStep;
        }

        u += uStep;
    }

    for (int i = 0; i<uDivisions - 1; i++)
    {
        for (int j = 0; j<vDivisions - 1; j++)
        {
            unsigned int p00 = i*vDivisions + j;
            unsigned int p01 = i*vDivisions + j + 1;
            unsigned int p10 = (i + 1)*vDivisions + j;
            unsigned int p11 = (i + 1)*vDivisions + j + 1;

            indices.push_back(p00);
            indices.push_back(p10);
            indices.push_back(p01);

            indices.push_back(p01);
            indices.push_back(p10);
            indices.push_back(p11);
        }
    }
}


void generatePSphere(vector<vec3>& positions, vector<vec3>& normals,
    vector<vec2>& uvs, vector<unsigned int>& indices,
    float r, vec3 center,
    int uDivisions, int vDivisions)
{
    // udivisions will be theta
    // vdivisions will be phi	
    float uStep = 1.f / (float)(uDivisions - 1);
    float vStep = 1.f / (float)(vDivisions - 1);

    float u = 0.f;

    // Iterate through phi and theta
    for (double phi = 0.; phi < uDivisions; phi++) // Azimuth [0, 2PI]
    {
        float v = 0.f;
        for (double theta = 0.; theta < vDivisions; theta++) // Elevation [0, PI]
        {
            vec3 point;
            point.x = r * cos(v * 2 * PI) * sin(u*PI) + center.x;
            point.y = r * sin(v * 2 * PI) * sin(u*PI) + center.y;
            point.z = r               * cos(u*PI) + center.z;

            vec3 normal = normalize(point - center);

            positions.push_back(point);
            normals.push_back(normal);
            uvs.push_back(vec2(u, v));

            v += vStep;
        }
        u += uStep;
    }

    for (int i = 0; i<uDivisions - 1; i++)
    {
        for (int j = 0; j<vDivisions - 1; j++)
        {
            unsigned int p00 = i*vDivisions + j;
            unsigned int p01 = i*vDivisions + j + 1;
            unsigned int p10 = (i + 1)*vDivisions + j;
            unsigned int p11 = (i + 1)*vDivisions + j + 1;

            indices.push_back(p00);
            indices.push_back(p10);
            indices.push_back(p01);

            indices.push_back(p01);
            indices.push_back(p10);
            indices.push_back(p11);
        }
    }

}
//Initialization
void initGL()
{
    //COMMENT/UNCOMMENT - essentially turns wire frames on and off
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //Only call these once - don't call again every time you change geometry
    generateIDs();		//Create VertexArrayObjects and Vertex Buffer Objects and store their handles
    initShader();		//Create shader and store program ID

    initVAO();			//Describe setup of Vertex Array Objects and Vertex Buffer Object

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    for (int i = 0; i < sizeof(*imageNames); i++) {
        textureIds[i] = createTexture(imageNames[i]);
    }
}

//Draws buffers to screen
void render(Camera* cam, mat4 perspectiveMatrix, mat4 modelview, int startElement, int numElements, GLuint texid)
{


    mat4 camMatrix = cam->getMatrix();

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "cameraMatrix"),
        1,
        false,
        &camMatrix[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "perspectiveMatrix"),
        1,
        false,
        &perspectiveMatrix[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "modelviewMatrix"),
        1,
        false,
        &modelview[0][0]);

    GLint uniformLocation = glGetUniformLocation(shader[SHADER::DEFAULT], "shade");
    glUniform1i(uniformLocation, shade);	//Normalize coordinates between 0 and 1

    loadTexture(texid, GL_TEXTURE0, shader[SHADER::DEFAULT], "image");


    CheckGLErrors("loadUniforms");

    glDrawElements(
        GL_TRIANGLES,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        numElements,		//How many indices
        GL_UNSIGNED_INT,	//Type
        (void*)0			//Offset
        );

    CheckGLErrors("render");
}

// ==========================================================================
// PROGRAM ENTRY POINT

Renderer::Renderer(int index)
{
	Renderer::index = index;
}

bool Renderer::initRenderer() {
	// initialize the GLFW windowing system
	if (!glfwInit()) {
		cout << "ERROR: GLFW failed to initilize, TERMINATING" << endl;
		return -1;
	}
	glfwSetErrorCallback(ErrorCallback);

	// attempt to create a window with an OpenGL 4.1 core profile context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(1024, 768, "CPSC 585 OpenGL Boilerplate", 0, 0);
	if (!window) {
		cout << "Program failed to create GLFW window, TERMINATING" << endl;
		glfwTerminate();
		return -1;
	}


	// set keyboard callback function and make our context current (active)
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetWindowSizeCallback(window, resizeCallback);
	glfwMakeContextCurrent(window);

	//Intialize GLAD
#ifndef LAB_LINUX
	if (!gladLoadGL())
	{
		cout << "GLAD init failed" << endl;
		return -1;
	}
#endif

	// query and print out information about our OpenGL environment
	QueryGLVersion();

	initGL();

	//SUN
	// Radius: 3 root scale on radius (then divided them all by 100): 0.8862 
	generatePSphere(points, normals, uvs, indices, .8862,earthCenter, 40, 40);

	//EARTH
	// Distance from the sun: 16 root scale: 3.24
	// Radius: 3 root scale on radius (then divided them all by 100): 0.1855 
	//generatePSphere(points2, normals2, uvs2, indices2, .2404f, earthCenter, 20, 20);
    generateTorus(points2, normals2, uvs2, indices2, .5404f, 0.3008f, 20, 20);
	//MOON 
	// Distance from the earth: 16 root scale: 2.42
	// Radius 3 root scale on radius (then divided them all by 100): 0.1202
	//generatePSphere(points3, normals3, uvs3, indices3, .1202f, moonCenter, 20, 20);

	//STARS
//	generatePSphere(points4, normals4, uvs4, indices4, 400.f, vec3(0, 0, 0), 100, 100);

	cam = Camera(vec3(0, 0, -1), vec3(0, 0, 5));
	

	//Don't need to call these on every draw, so long as they don't change
	glUseProgram(shader[SHADER::DEFAULT]);		//Use LINE program
	glBindVertexArray(vao[VAO::GEOMETRY]);		//Use the LINES vertex array

	glUseProgram(shader[SHADER::DEFAULT]);

}

void Renderer::drawScene()
    {
		//float fovy, float aspect, float zNear, float zFar
		mat4 perspectiveMatrix = perspective(radians(80.f), 1.f, 0.1f, 440.f);
        glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clear color and depth buffers (Haven't covered yet)

        loadBuffer(points, normals, uvs, indices);

        // call function to draw our scene

        shade = 1;
        render(&cam, perspectiveMatrix, mat4(1.f), 0, indices.size(), textureIds[0]);

        shade = 0;
        loadBuffer(points2, normals2, uvs2, indices2);
        render(&cam, perspectiveMatrix, mat4(1.f), 0, indices2.size(), textureIds[1]);

       // loadBuffer(points3, normals3, uvs3, indices3);
      //  render(&cam, perspectiveMatrix, mat4(1.f), 0, indices3.size(), textureIds[2]);

        shade = 0;
      //  loadBuffer(points4, normals4, uvs4, indices4);
     //   render(&cam, perspectiveMatrix, mat4(1.f), 0, indices4.size(), textureIds[3]);

        // scene is rendered to the back buffer, so swap to front for display
        glfwSwapBuffers(window);

        // sleep until next event before drawing again
        glfwPollEvents();
}

Renderer::~Renderer()
{
    // clean up allocated resources before exit
    deleteIDs();
    glfwDestroyWindow(window);
    glfwTerminate();

}

bool Renderer::shouldClose()
{
	if (glfwWindowShouldClose(window))
		return true;
	else return false;
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
