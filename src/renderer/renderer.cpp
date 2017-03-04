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
#include <math.h>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std;
using namespace glm;

Renderer::Renderer(int index, vec3 pos, vec3 dir) :
    cam(new Camera(vec3(0, -1, -1), vec3(0, 10, 10)))
{
    index = index;
    light = new Light(pos, dir);
}

void Renderer::renderShadowMap(const std::vector<Entity*>& ents) {

     //Changing size can drastically affect the shadow map.
    // Smaller values capture less of the area but do allow for better shadows.
    // important area for tuning - especially in relation to mapsize 
    float size = 500; // 900
   
    // Compute the MVP matrix from the light's point of view 
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-size, size, -size, size, 0.1f, 1500.f); // 1500.0f
    glm::mat4 depthViewMatrix = glm::lookAt(light->getPos(), glm::vec3(0.f), glm::vec3(0, 1, 0));
    depthMVP = depthProjectionMatrix * depthViewMatrix;

    // Use our shader
    glUseProgram(shader[SHADER::SHADOW]);

    //Change the viewport and frame buffer, clear the buffer bit
    glViewport(0, 0, 8192, 8192);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // Render each of the entities to the shadowMap texture in a first pass
    for (auto& e : ents) {
        // This is virtual function lookup for each entity, might be slow
        // Potentially optimize by using a single vec of Renderables
        if (e->canRender()) {
            // Careful here - static_cast is FAST, but potentially dangerous if an entity
            // hasn't been initialized properly
            Renderable* r = static_cast<Renderable*>(e);

            for (Model* model : r->getModels()) {
                mat4 scale = model->get_scaling();
                mat4 rot = glm::mat4_cast(r->getQRot());
                mat4 trans = glm::translate(mat4(), r->getPos());
                mat4 mmatrix = trans * rot * scale;

                shrender(*model,mmatrix, 0);
            }
        }
        
     }
    //Back to base frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Sets up the frame buffer and the shadowMap texture
bool Renderer::loadFrameBuffers() {
    glGenFramebuffers(1, &FramebufferName);
    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 8192, 8192, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);// To gl linear possibly
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, depthTexture, 0);

    glDrawBuffer(GL_NONE); // No color buffer is drawn to.
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;
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

// Shadow render - does the rendering to the texture - called by renderShadowMap
void Renderer::shrender(const Model& model, mat4 model_matrix, int startElement){

    // Make sure depth testing is enabled and cull front faces
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SHADOW], "depthMVP"), 1, false, &depthMVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SHADOW], "model"), 1, false, &model_matrix[0][0]);

    // Set object-specific VAO
    glBindVertexArray(model.vao[VAO::GEOMETRY]);
    glDrawElements(
    GL_TRIANGLES,		    //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
    model.num_indices(),    //How many indices
    GL_UNSIGNED_INT,	    //Type
    (void*)0			    //Offset
    );

    //Switch back to the original frame buffer and reset face culling to back face culling
    glCullFace(GL_BACK);
    glBindVertexArray(0);
}


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


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "shadowMap"), 1);
   
    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "depthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthMVP));

    glUniform3f(glGetUniformLocation(shader[SHADER::DEFAULT], "lightPos"), light->getPos().x, light->getPos().y, light->getPos().z);
    glUniform3f(glGetUniformLocation(shader[SHADER::DEFAULT], "lightDir"), light->getDir().x, light->getDir().y, light->getDir().z);
    glUniform3fv(glGetUniformLocation(shader[SHADER::DEFAULT], "viewPos"), 1, &cam->pos[0]);	

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

void Renderer::drawScene(const std::vector<Entity*>& ents)
{
    renderShadowMap(ents);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, 1024, 768); // Render on the whole framebuffer, complete from the lower left corner to the upper right

  //  glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
    
    cam->update();
	//float fovy, float aspect, float zNear, float zFar
	mat4 perspectiveMatrix = perspective(radians(80.f), 1.f, 0.1f, 3000.f);
    glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clear color and depth buffers (Haven't covered yet)

    glUseProgram(shader[SHADER::SKYBOX]);
    drawSkybox(this->skybox, perspectiveMatrix);
    
    glUseProgram(shader[SHADER::DEFAULT]);

    ///////////////////////////////////////
    //glm::mat4 depthBiasMVP = depthMVP;
    //glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "depthBiasMVP"), 1, GL_FALSE, &depthBiasMVP[0][0]);
    ///////////////////////////////////////

    for (auto& e : ents) {
        // This is virtual function lookup for each entity, might be slow
        // Potentially optimize by using a single vec of Renderables
        if (e->canRender()) {
            // Careful here - static_cast is FAST, but potentially dangerous if an entity
            // hasn't been initialized properly
            Renderable* r = static_cast<Renderable*>(e);
			
            for (Model* model : r->getModels()) {
                mat4 scale = model->get_scaling();
                mat4 rot = glm::mat4_cast(r->getQRot());
                mat4 trans = glm::translate(mat4(), r->getPos());
                mat4 mmatrix = trans * rot * scale;

                render(*model, perspectiveMatrix, mmatrix, 0);
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