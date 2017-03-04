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

Renderer::Renderer(int index) :
    cam(new Camera(vec3(0, -1, -1), vec3(0, 10, 10)))
{
    index = index;
    light = new Light(glm::vec3(200, 400, 200), glm::vec3(-400, -500, -100)); // TODO: stop hard-coding this
}

Renderer::~Renderer()
{
    delete cam;
}

void Renderer::postGLInit() {
    initSkybox();
    initDepthFrameBuffer(SM_frameBuffer, SM_depthTex);
    initDepthFrameBuffer(SIL_frameBuffer, SIL_depthTex);
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
    glBindFramebuffer(GL_FRAMEBUFFER, SM_frameBuffer);
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

                addToShadowMap(*model, mmatrix, 0);
            }
        }
        
     }
    //Back to base frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Sets up the frame buffer and the shadowMap texture
bool Renderer::initDepthFrameBuffer(GLuint &frameBuffer, GLuint &depthTex) {
    glGenFramebuffers(1, &frameBuffer);
    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, 8192, 8192, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);// To gl linear possibly
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, depthTex, 0);

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
void Renderer::addToShadowMap(const Model& model, mat4 model_matrix, int startElement){

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


void Renderer::renderModel(const Model& model, mat4 &perspectiveMatrix, glm::mat4 scale, glm::mat4 rot, glm::mat4 trans)
{
   // drawSil(model, perspectiveMatrix, scale, rot, trans);

    drawShade(model, perspectiveMatrix, scale, rot, trans);
}

void Renderer::drawShade(const Model& model, mat4 &perspectiveMatrix, glm::mat4 scale, glm::mat4 rot, glm::mat4 trans) {
    glUseProgram(shader[SHADER::DEFAULT]);

    glBindVertexArray(model.vao[VAO::GEOMETRY]);
    mat4 &camMatrix = cam->getMatrix();
    mat4 model_matrix = trans * rot * scale;

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
    glBindTexture(GL_TEXTURE_2D, SM_depthTex);
    glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "shadowMap"), 1);
   
    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "depthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthMVP));

    glUniform3f(glGetUniformLocation(shader[SHADER::DEFAULT], "lightPos"), light->getPos().x, light->getPos().y, light->getPos().z);
    glUniform3f(glGetUniformLocation(shader[SHADER::DEFAULT], "lightDir"), light->getDir().x, light->getDir().y, light->getDir().z);
    glUniform3fv(glGetUniformLocation(shader[SHADER::DEFAULT], "viewPos"), 1, &cam->pos[0]);	

    model.getTex()->load(GL_TEXTURE0, shader[SHADER::DEFAULT], "image");
    CheckGLErrors("loadUniforms");

    glDrawElements(
        GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        model.num_indices(), //How many indices
        GL_UNSIGNED_INT,	 //Type
        (void*)0			 //Offset
        );

    CheckGLErrors("drawShade");
    glBindVertexArray(0);
}

void Renderer::drawSil(const Model& model, mat4 &perspectiveMatrix, glm::mat4 scale, glm::mat4 rot, glm::mat4 trans) {
    glUseProgram(shader[SHADER::SIL]);

    glBindVertexArray(model.vao[VAO::GEOMETRY]);
    mat4 &camMatrix = cam->getMatrix();

    float sil_scale = 1.1;
    mat4 model_matrix = trans * rot * scale;
    mat4 scaled_model = trans * rot * (scale * glm::scale(glm::vec3(sil_scale, sil_scale, sil_scale)));

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SIL], "cameraMatrix"),
        1,
        false,
        &camMatrix[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SIL], "perspectiveMatrix"),
        1,
        false,
        &perspectiveMatrix[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SIL], "modelviewMatrix"),
        1,
        false,
        &scaled_model[0][0]);

    CheckGLErrors("loadUniforms");


    // Draw black silhouette
   // glEnable(GL_CULL_FACE); // enable culling
    glDepthMask(GL_FALSE); // enable writes to Z-buffer
   // glEnable(GL_DEPTH_TEST);
   glDisable(GL_DEPTH_TEST);


 //   glDisable(GL_DEPTH_TEST);
  //  glDepthFunc(GL_GEQUAL);
    glUniform3f(glGetUniformLocation(shader[SHADER::SIL], "u_color1"), 0.0, 0.0, 0.0);
    glUniform1f(glGetUniformLocation(shader[SHADER::SIL], "u_offset1"), 0.0f); // line thickness
    glDrawElements(
        GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        model.num_indices(), //How many indices
        GL_UNSIGNED_INT,	 //Type
        (void*)0			 //Offset
        );

    /*
    // Reset scaling
    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SIL], "modelviewMatrix"),
        1,
        false,
        &model_matrix[0][0]);


    // Draw white center
    glDepthMask(GL_TRUE); // disable writes to Z-buffer
    glEnable(GL_DEPTH_TEST);

   // glDepthFunc(GL_LESS);
    glUniform3f(glGetUniformLocation(shader[SHADER::SIL], "u_color1"), 1.0, 1.0, 1.0);
    glUniform1f(glGetUniformLocation(shader[SHADER::SIL], "u_offset1"), 0.0f); // line thickness
        glDrawElements(
        GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        model.num_indices(), //How many indices
        GL_UNSIGNED_INT,	 //Type
        (void*)0			 //Offset
        );*/

  //  glDepthMask(GL_TRUE); // disable writes to Z-buffer
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE); // enable writes to Z-buffer
    glEnable(GL_DEPTH_TEST);

    CheckGLErrors("drawSil");
    glBindVertexArray(0);
}


void Renderer::drawScene(const std::vector<Entity*>& ents)
{
    renderShadowMap(ents);

    glViewport(0, 0, width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

  //  glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
    
    cam->update();
	//float fovy, float aspect, float zNear, float zFar
    mat4 perspectiveMatrix = cam->calcPerspective();
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

                renderModel(*model, perspectiveMatrix, scale, rot, trans);
            }
        }
    }
}

// This seems kinda dangerous
Camera* Renderer::getCam() {
    return cam;
}

void Renderer::setDims(int width, int height) {
    this->width = width;
    this->height = height;
    cam->setDims(width, height);
}
