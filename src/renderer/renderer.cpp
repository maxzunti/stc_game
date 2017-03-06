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
#include "text2D.h"

#include <GLFW/glfw3.h>
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
    // (glm::vec3(200, 400, 200)
    light = new Light(glm::vec3(200, 600, 350), glm::vec3(-400, -500, -100)); // TODO: stop hard-coding this
}

Renderer::~Renderer()
{
    delete cam;
}

void Renderer::postGLInit() {
    initSkybox();
    initDepthFrameBuffer(SM_frameBuffer, SM_depthTex, SM_res, SM_res);

    initDepthFrameBuffer(SIL_frameBuffer1, SIL_depthTex, width, height);
    initDepthFrameBuffer(SIL_frameBuffer2, SIL_depthTex, width, height);
    initText();
}

void Renderer::initText() {
    blueText = new Text2D("assets/textures/blue_gg_font.png");
    blackText = new Text2D("assets/textures/black_gg_font.png");
    redText = new Text2D("assets/textures/red_gg_font.png");
    whiteText = new Text2D("assets/textures/white_gg_font.png");
}

// Sets up the frame buffer and the shadowMap texture
bool Renderer::initDepthFrameBuffer(GLuint &frameBuffer, GLuint &depthTex, int width, int height) {
    glGenFramebuffers(1, &frameBuffer);
    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // reset framebuffer
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
    CheckGLErrors("skybox loadUniforms");

    glDrawArrays(GL_TRIANGLES, 0, 36);

    CheckGLErrors("drawSkybox");
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
}


void Renderer::renderShadowMap(const std::vector<Entity*>& ents) {

    //Changing size can drastically affect the shadow map.
    // Smaller values capture less of the area but do allow for better shadows.
    // important area for tuning - especially in relation to mapsize 
    float size = 860; // 900

    // Compute the MVP matrix from the light's point of view 
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-size, 690, -size, size, 1.f, 1700.f); // 1500.0f
    glm::mat4 depthViewMatrix = glm::lookAt(light->getPos(), glm::vec3(0.f), glm::vec3(0, 1, 0));
    depthMVP = depthProjectionMatrix * depthViewMatrix;

    // Use our shader
    glUseProgram(shader[SHADER::SHADOW]);

    //Change the viewport and frame buffer, clear the buffer bit
    glViewport(0, 0, SM_res, SM_res);
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

// Shadow render - does the rendering to the texture - called by renderShadowMap
void Renderer::addToShadowMap(const Model& model, mat4 model_matrix, int startElement){

    // Make sure depth testing is enabled and cull front faces
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
  //  glCullFace(GL_BACK);

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
    if (model.shouldSil()) {
        const mat4 model_matrix = trans * rot * scale;
        mat4 sil_model;

        glBindFramebuffer(GL_FRAMEBUFFER, SIL_frameBuffer1);
        glClear(GL_DEPTH_BUFFER_BIT); // clear FB1
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // copy from default...
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, SIL_frameBuffer1); // ...to sil_1
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
            GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // reset framebuffer

        if (model.sil_jitter > 0.0) { // sil_jitter set; use the more expensive jitter technique
                                      // Copy pre-sil depth-buffer values to temp SIL_frameBuffer1

            // +X
            mat4 jitter_trans = glm::translate(vec3(model.sil_jitter, 0.0, 0.0));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);

            // -X
            jitter_trans = glm::translate(vec3(-model.sil_jitter, 0.0, 0.0));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);

            // +Y
            jitter_trans = glm::translate(vec3(0.0, model.sil_jitter, 0.0));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);

            // -Y
            jitter_trans = glm::translate(vec3(0.0, -model.sil_jitter, 0.0));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);

            // +Z
            jitter_trans = glm::translate(vec3(0.0, 0.0, model.sil_jitter));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);

            // -Z
            jitter_trans = glm::translate(vec3(0.0, 0.0, -model.sil_jitter));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);
        }
        else {
            sil_model = trans * rot * (scale * glm::scale(glm::vec3(model.sil_x, model.sil_y, model.sil_z)));
            drawSil(model, perspectiveMatrix, sil_model);
        }

        // Copy post-sil depth-buffer values to temp SIL_frameBuffer2
        glBindFramebuffer(GL_FRAMEBUFFER, SIL_frameBuffer2);
        glClear(GL_DEPTH_BUFFER_BIT); // clear FB2
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0); // copy from default...
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, SIL_frameBuffer2); // ...to sil_2
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
            GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // reset framebuffer
    }
    drawShade(model, perspectiveMatrix, scale, rot, trans);
}

void Renderer::drawShade(const Model& model, mat4 &perspectiveMatrix, glm::mat4 scale, glm::mat4 rot, glm::mat4 trans) {
    glUseProgram(shader[SHADER::DEFAULT]);

    glEnablei(GL_BLEND, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
    CheckGLErrors("loadUniforms in render");

    if (model.shouldSil()) {
        // Shade against the cached pre-silhouette depth values
        glBindFramebuffer(GL_READ_FRAMEBUFFER, SIL_frameBuffer1); // pre-sil depth
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // to actual drawbuffer
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
            GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // reset framebuffer

  //  glEnablei(GL_BLEND, 0);
  //  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawElements(
        GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        model.num_indices(), //How many indices
        GL_UNSIGNED_INT,	 //Type
        (void*)0			 //Offset
        );

    if (model.shouldSil()) {
        // Restore post-sil depth values to drawbuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, SIL_frameBuffer2); // post-sil depth
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // to actual drawbuffer
        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height,
            GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // reset framebuffer
    }

    CheckGLErrors("drawShade");
    glDisablei(GL_BLEND, 0);
    glBindVertexArray(0);
}

void Renderer::drawSil(const Model& model, mat4 &perspectiveMatrix, glm::mat4 &mmatrix) {
    glUseProgram(shader[SHADER::SIL]);

    glBindVertexArray(model.vao[VAO::GEOMETRY]);
    mat4 &camMatrix = cam->getMatrix();

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
        &mmatrix[0][0]);

    CheckGLErrors("loadUniforms");

    glDepthMask(GL_TRUE); // enable writes to Z-buffer

    glUniform3f(glGetUniformLocation(shader[SHADER::SIL], "u_color1"), 0.0, 0.0, 0.0);

    // Still using default framebuffer => silhouettes update default framebuffer
    glDrawElements(
        GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        model.num_indices(), //How many indices
        GL_UNSIGNED_INT,	 //Type
        (void*)0			 //Offset
        );

    CheckGLErrors("drawSil");
    glBindVertexArray(0);
}

void Renderer::drawScene(const std::vector<Entity*>& ents)
{
    renderShadowMap(ents);

    glViewport(0, 0, width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    
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
    drawText();
}

void Renderer::drawText() {
    // Draw text here

    int xPlacement = 30;
    int yPlacement = this->height - 100;
    int ds_offset = 3;

    //Lap Placement - insert real lap information here

    char lapText[256];
    sprintf(lapText, "\n%i\\3",cam->getCar()->getLap());
    drawDropShadowText("LAP", blueText, blackText, xPlacement, yPlacement, 60, ds_offset);
    drawDropShadowText(lapText, whiteText, blackText, xPlacement, yPlacement, 60, ds_offset);

    //Timer Text - insert real timer info here
    xPlacement = 30;
    yPlacement = 100;
    char timeText[256];
    double currentTime = glfwGetTime();
    sprintf(timeText, "\n%.2i:%.2i", int(currentTime)/60,int(currentTime)%60);
    drawDropShadowText("TIME", blueText, blackText, xPlacement, yPlacement, 60, ds_offset);
    drawDropShadowText(timeText, whiteText, blackText, xPlacement, yPlacement, 60, ds_offset);

    //Position - insert real position info here
    xPlacement = this->width - 300;
    yPlacement = this->height - 150;

    char posText[256];
    sprintf(posText, "1ST");
    drawDropShadowText(posText, whiteText, blackText, xPlacement, yPlacement, 100, ds_offset);

    xPlacement = this->width - 300;
    yPlacement = 50;
    int speed_int = (int)cam->getCarSpeed();
    std::string speed_str = std::to_string(speed_int);
    drawDropShadowText(speed_str.c_str(), whiteText, blackText, xPlacement, yPlacement, 100, ds_offset);

    if ((cam->getCar()->getLap() == 3) && (cam->getCar()->getPartOfLap() == 4)) {
        xPlacement = width/4;
        yPlacement = height-200;
        char winText[256];
        sprintf(winText, "YOU WIN!");
        drawDropShadowText(winText, whiteText, blackText, xPlacement, yPlacement, 100, ds_offset);
    }

}

void Renderer::drawDropShadowText(const char* string, Text2D* front, Text2D* back, int x, int y, int size, int offset) {
    back->printText2D(string, x, y, size, this->width, this->height);
    front->printText2D(string, x + offset, y - offset, size, this->width, this->height);
}


// This seems kinda dangerous
Camera* Renderer::getCam() {
    return cam;
}

void Renderer::setDims(int width, int height) {
    this->width = width;
    this->height = height;
    cam->setDims(width, height);

    // Need to resize silhouette FB to match depth-buffer res
    glDeleteFramebuffers(1, &SIL_frameBuffer1);
    glDeleteFramebuffers(1, &SIL_frameBuffer2);
    glDeleteTextures(1, &SIL_depthTex);
    initDepthFrameBuffer(SIL_frameBuffer1, SIL_depthTex, width, height);
    initDepthFrameBuffer(SIL_frameBuffer2, SIL_depthTex, width, height);
}
