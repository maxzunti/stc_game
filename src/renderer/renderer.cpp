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

int shade = 0;

void Renderer::render(const Model& model, mat4 &perspectiveMatrix, mat4 model_matrix, int startElement)
{
    // Set object-specific VAO
    glBindVertexArray(model.vao[VAO::GEOMETRY]);

    mat4 &camMatrix = cam->getMatrix();

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
        &model_matrix[0][0]);

    GLint uniformLocation = glGetUniformLocation(shader[SHADER::DEFAULT], "shade");
    glUniform1i(uniformLocation, shade);	//Normalize coordinates between 0 and 1

    model.getTex()->load(GL_TEXTURE0, shader[SHADER::DEFAULT], "image");

    CheckGLErrors("loadUniforms");

    glDrawElements(
        GL_TRIANGLES,		//What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        model.num_indices(),		//How many indices
        GL_UNSIGNED_INT,	//Type
        (void*)0			//Offset
        );

    CheckGLErrors("render");
    glBindVertexArray(0);
}

void Renderer::drawSkybox(const Skybox* sb, glm::mat4 &perspectiveMatrix)
{
    glDepthMask(GL_FALSE);

    // Set object-specific VAO
    glBindVertexArray(sb->vao);

    glm::mat4 view = glm::mat4(glm::mat3(cam->getMatrix())); // Convert to 3x3 to remove translation components

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SKYBOX], "view"),
        1,
        false,
        &view[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SKYBOX], "projection"),
        1,
        false,
        &perspectiveMatrix[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sb->tex_id);

 //   GLuint uniformLocation = glGetUniformLocation(shader[SHADER::SKYBOX], skybox);
 //   glUniform1i(uniformLocation, 0);
    CheckGLErrors("loadUniforms");

    glDrawArrays(GL_TRIANGLES, 0, 36);

    CheckGLErrors("drawSkybox");
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}


Renderer::Renderer(int index) :
    cam(new Camera(vec3(0, 0, -1), vec3(0, 0, 5)))
{
	Renderer::index = index;
}

void Renderer::initSkybox() {
    // Maybe generalize this at some point, but hard-coding it should be okay for now
    std::string skybox_dir = "assets\\skybox\\miramar\\";
    std::string filenames[6] = {
        skybox_dir + "miramar_rt.tga",
        skybox_dir + "miramar_lf.tga",
        skybox_dir + "miramar_up.tga",
        skybox_dir + "miramar_dn.tga",
        skybox_dir + "miramar_bk.tga",
        skybox_dir + "miramar_ft.tga",
    };
    skybox = new Skybox(filenames);
}

void Renderer::drawScene(const std::vector<Entity*>& ents)
{
	//float fovy, float aspect, float zNear, float zFar
	mat4 perspectiveMatrix = perspective(radians(80.f), 1.f, 0.1f, 440.f);
    glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clear color and depth buffers (Haven't covered yet)

    shade = 0;
    if (skybox) { // kill me - ifs are expensive
        std::cout << "drawing skybox?" << std::endl;
        glUseProgram(shader[SHADER::SKYBOX]);
        drawSkybox(this->skybox, perspectiveMatrix);
    }

    glUseProgram(shader[SHADER::DEFAULT]);
    for (const auto& e : ents) {
        // This is virtual function lookup for each entity, might be slow
        // Potentially optimize by using a single vec of Renderables
        if (e->canRender()) {
            // Careful here - static_cast is FAST, but potentially dangerous if an entity
            // hasn't been initialized properly
            const Renderable* r = static_cast<Renderable*>(e);
			
			// This one seems to rotate globally, not locally... but haven't tested enough
		/*	glm::mat4 mmatrix = glm::translate(glm::rotate(glm::rotate(glm::rotate(r->getModel()->get_scaling(),
				r->getRot().x, vec3(1, 0, 0)),
				r->getRot().y, vec3(0, 1, 0)),
				r->getRot().z, vec3(0, 0, 1)), 
				r->getPos());  */

			// This one seems to rotate locally (properly)
			glm::mat4 mmatrix = glm::rotate(glm::rotate(glm::rotate(glm::translate(r->getModel()->get_scaling(), r->getPos()),
				r->getRot().x, vec3(1, 0, 0)),
				r->getRot().y, vec3(0, 1, 0)),
				r->getRot().z, vec3(0, 0, 1));

            if (r->is_model_loaded()) {
                render(*r->getModel(), perspectiveMatrix, mmatrix, 0);
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