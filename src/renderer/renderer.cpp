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
#include "renderer.h"
#include "camera.h"

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

GLuint textureIds[4];
int shade = 0;

//Draws buffers to screen
// TODO:  Simplify my signature to make better use of the Model struct
void Renderer::render(const GLuint vao[VAO::COUNT], mat4 perspectiveMatrix, mat4 modelview, int startElement, int numElements, GLuint texid)
{
    // Set object-specific VAO
    glBindVertexArray(vao[VAO::GEOMETRY]);

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

    // loadTexture(texid, GL_TEXTURE0, shader[SHADER::DEFAULT], "image");

    CheckGLErrors("loadUniforms");

    glDrawElements(
        GL_TRIANGLES,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        numElements,		//How many indices
        GL_UNSIGNED_INT,	//Type
        (void*)0			//Offset
        );

    CheckGLErrors("render");
}

Renderer::Renderer(int index) :
    cam(new Camera(vec3(0, 0, -1), vec3(0, 0, 5)))
{
	Renderer::index = index;
}

bool Renderer::initRenderer() {
	//Don't need to call these on every draw, so long as they don't change
	glUseProgram(shader[SHADER::DEFAULT]);		//Use LINE program
	//glBindVertexArray(vao[VAO::GEOMETRY]);		//Use the LINES vertex array

	glUseProgram(shader[SHADER::DEFAULT]);

    return 0;
}

void Renderer::drawScene(const std::vector<Entity*>& ents)
{
	//float fovy, float aspect, float zNear, float zFar
	mat4 perspectiveMatrix = perspective(radians(80.f), 1.f, 0.1f, 440.f);
    glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clear color and depth buffers (Haven't covered yet)

    shade = 0;
    

    for (const auto& e : ents) {
        // This is virtual function lookup for each entity, might be slow
        // Potentially optimize by using a single vec of Renderables
        if (e->canRender()) {
            // Careful here - static_cast is FAST, but potentially dangerous if an entity
            // hasn't been initialized properly
            const Renderable* r = static_cast<Renderable*>(e);
			glm::mat4 scale = glm::translate(glm::scale(vec3(0.5f, 0.5f, 0.5)),vec3(e->xPos(), e->yPos(), e->zPos()));     // Currently shrinking everything (I added translation here)
            if (r->is_model_loaded()) {
                render(r->getModel()->vao, perspectiveMatrix, scale, 0, r->getModel()->indices.size(), textureIds[0]);
            }
        }
    }
}

Renderer::~Renderer()
{
    delete cam;
}

// This seems kinda dangerous
Camera* Renderer::getCam() {
    return cam;
}