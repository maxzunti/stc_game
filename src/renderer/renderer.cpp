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
#include "../entity/Track.h"
#include "../entity/Wheel.h"
#include "../entity/Entity.h"

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
#include <glm/gtx/rotate_vector.hpp>

#include <omp.h>

using namespace std;
using namespace glm;

GLuint Renderer::SM_depthTex;
glm::mat4 Renderer::depthMVP;
struct Stencil {
    const static int none = 0x00;
    const static int sil = 0x01;
    const static int track = 0x02;
    const static int refDrawn = 0x04;
    const static int clear = 0xFF;
};

Renderer::Renderer(int index) :
    cam(new Camera(vec3(0, -1, -1), vec3(0, 10, 10)))
{
    this->index = index;
    // (glm::vec3(200, 400, 200)
    light = new Light(glm::vec3(200, 600, 350), glm::vec3(-400, -500, -100)); // TODO: stop hard-coding this
}

Renderer::~Renderer()
{
    delete cam;
    glDeleteFramebuffers(1, &mm_frameBuffer);
    glDeleteFramebuffers(1, &mm_pips_frameBuffer);
    glDeleteTextures(1, &mm_tex);
    glDeleteTextures(1, &mm_pips_tex);
    if (index == 0) {
        glDeleteFramebuffers(1, &SM_frameBuffer);
        glDeleteTextures(1, &SM_depthTex);
        glDeleteFramebuffers(1, &skyline_mm_buffer);
        glDeleteTextures(1, &skyline_mm_tex);
    }

}

void Renderer::postGLInit() {
   // initSkybox();
    initText();

    if (index == 0) {
        initColorFrameBuffer(mm_frameBuffer, mm_tex, mmSize, mmSize);
        initColorFrameBuffer(mm_pips_frameBuffer, mm_pips_tex, mmSize, mmSize);
        initColorFrameBuffer(skyline_mm_buffer, skyline_mm_tex, skyline_mm_size, skyline_mm_size);
    }
}

void Renderer::initFrameBuffer() {
    initDepthFrameBuffer(SM_frameBuffer, Renderer::SM_depthTex, SM_res, SM_res);
}

void Renderer::initText() {
    blueText = new Text2D("assets/textures/blue_gg_font.png");
    blackText = new Text2D("assets/textures/black_gg_font.png");
    redText = new Text2D("assets/textures/red_gg_font.png");
    whiteText = new Text2D("assets/textures/white_gg_font.png");
    mmPips = new Text2D("assets/textures/mm_icons.png");

    blackBar = new Text2D("assets/textures/blackbar.png");

    hookReady = new Text2D("assets/textures/hookcdcharged.png");
    hookNotReady1 = new Text2D("assets/textures/hookcdicondepleted1.png");
    hookNotReady2 = new Text2D("assets/textures/hookcdicondepleted2.png");
    hookNotReady3 = new Text2D("assets/textures/hookcdicondepleted3.png");

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

// Not actually being used
bool Renderer::initColorFrameBuffer(GLuint &frameBuffer, GLuint &colorTex, int width, int height) {
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);// To gl linear possibly
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        return false;

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // reset framebuffer
}

void Renderer::initSkybox(int track) {
    // Maybe generalize this at some point, but hard-coding it should be okay for now

    if (track == 1 || track == 0) {
        std::string skybox_dir = "assets\\skybox\\miramar\\";
        std::string filenames[6] = {
            skybox_dir + "Right.png",
            skybox_dir + "Left.png",
            skybox_dir + "Up.png",
            skybox_dir + "Down.png",
            skybox_dir + "Back.png",
            skybox_dir + "Front.png",
        };

        skybox = new Skybox(filenames);
    }
    else if (track == 2) {
        std::string skybox_dir = "assets\\skybox\\ie\\";
        std::string filenames[6] = {
            skybox_dir + "Right.tga",
            skybox_dir + "Left.tga",
            skybox_dir + "Up.tga",
            skybox_dir + "Down.tga",
            skybox_dir + "Back.tga",
            skybox_dir + "Front.tga",
        };

        skybox = new Skybox(filenames);
    }
}

void Renderer::drawSkybox(glm::mat4 &perspectiveMatrix)
{
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    // Set object-specific VAO
    glBindVertexArray(skybox->vao);

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
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->tex_id);

    //   GLuint uniformLocation = glGetUniformLocation(shader[SHADER::SKYBOX], skybox);
    //   glUniform1i(uniformLocation, 0);
    CheckGLErrors("skybox loadUniforms");

    glDrawArrays(GL_TRIANGLES, 0, 36);

    CheckGLErrors("drawSkybox");
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}


void Renderer::renderShadowMap(const std::vector<Renderable*>& ents) {

    //Changing size can drastically affect the shadow map.
    // Smaller values capture less of the area but do allow for better shadows.
    // important area for tuning - especially in relation to mapsize 
    float size =(track==1)?860:3000; // 900 map 1: 860

    // Compute the MVP matrix from the light's point of view 

    glm::mat4 depthProjectionMatrix;
    if (track == 1) {
        depthProjectionMatrix = glm::ortho<float>(-size, 690, -size, size, 1.f, 1700.f); // 1700.0f 1500.0f // map 1: 1700, 690 in second place
        light->setPos(glm::vec3(200, 600, 350));
    }
    else {
        depthProjectionMatrix = glm::ortho<float>(-size, size, -size, size, 1.f, 2700.f); // 1700.0f 1500.0f // map 1: 1700, 690 in second place
        light->setPos(normalize(light->getPos()) * vec3(2000));
    }

    glm::mat4 depthViewMatrix = glm::lookAt(light->getPos(), glm::vec3(0.f), glm::vec3(0, 1, 0));
    Renderer::depthMVP = depthProjectionMatrix * depthViewMatrix;

    // Use our shader
    glUseProgram(shader[SHADER::SHADOW]);

    //Change the viewport and frame buffer, clear the buffer bit
    glViewport(0, 0, SM_res, SM_res);
    glBindFramebuffer(GL_FRAMEBUFFER, SM_frameBuffer);
    glClear(GL_DEPTH_BUFFER_BIT);

    // Render each of the entities to the shadowMap texture in a first pass
    #pragma omp parallel for
    for (auto &it = ents.begin(); it < ents.end(); it++) {
        Renderable* r = *it;
        if (!r->canRender()) {
            continue;
        }
        for (Model* model : r->getModels()) {
            mat4 scale = model->get_scaling();
            mat4 rot = glm::mat4_cast(r->getQRot());
            mat4 trans = glm::translate(mat4(), r->getPos());
            mat4 mmatrix = trans * rot * scale;

            addToShadowMap(*model, mmatrix, 0);
        }
    }
    //Back to base frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Shadow render - does the rendering to the texture - called by renderShadowMap
void Renderer::addToShadowMap(const Model& model, mat4 model_matrix, int startElement){

    // Make sure depth testing is enabled and cull front faces
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
  //  glCullFace(GL_BACK);

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SHADOW], "depthMVP"), 1, false, &Renderer::depthMVP[0][0]);
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
   // glCullFace(GL_BACK);
    glBindVertexArray(0);
}


void Renderer::renderModel(const Model& model, mat4 &perspectiveMatrix, glm::mat4 scale, glm::mat4 rot, glm::mat4 trans)
{
   drawShade(model, perspectiveMatrix, scale, rot, trans);
    if (model.shouldSil()) {
        const mat4 model_matrix = trans * rot * scale;
        mat4 sil_model;

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
            

            // More expensive, marginally better
            /*
            // X
            mat4 jitter_trans = glm::translate(vec3(model.sil_jitter, model.sil_jitter, 0.0));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);
            jitter_trans = glm::translate(vec3(model.sil_jitter, -model.sil_jitter, 0.0));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);
            jitter_trans = glm::translate(vec3(-model.sil_jitter, model.sil_jitter, 0.0));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);
            jitter_trans = glm::translate(vec3(-model.sil_jitter, -model.sil_jitter, 0.0));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);

            // Y
            jitter_trans = glm::translate(vec3(model.sil_jitter, 0.0, model.sil_jitter));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);
            jitter_trans = glm::translate(vec3(model.sil_jitter, 0.0, -model.sil_jitter));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);
            jitter_trans = glm::translate(vec3(-model.sil_jitter, 0.0, model.sil_jitter));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);
            jitter_trans = glm::translate(vec3(-model.sil_jitter, 0.0, -model.sil_jitter));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);

            // Z
            jitter_trans = glm::translate(vec3(0.0, model.sil_jitter, model.sil_jitter));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);
            jitter_trans = glm::translate(vec3(0.0, model.sil_jitter, -model.sil_jitter));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);
            jitter_trans = glm::translate(vec3(0.0, -model.sil_jitter, model.sil_jitter));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);
            jitter_trans = glm::translate(vec3(0.0, -model.sil_jitter, -model.sil_jitter));
            sil_model = jitter_trans * model_matrix;
            drawSil(model, perspectiveMatrix, sil_model);*/
            
        }
        else {
            sil_model = trans * rot * (scale * glm::scale(glm::vec3(model.sil_x, model.sil_y, model.sil_z)));
            drawSil(model, perspectiveMatrix, sil_model);
        }
    }
}

void Renderer::drawShade(const Model& model, mat4 &perspectiveMatrix, glm::mat4 scale, glm::mat4 rot, glm::mat4 trans, bool reflects, bool flipUV) {
    glUseProgram(shader[SHADER::DEFAULT]);

    glBindVertexArray(model.vao[VAO::GEOMETRY]);

    glEnable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->tex_id);
    glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "skybox"), 2);
    model.getTex()->load(GL_TEXTURE0, shader[SHADER::DEFAULT], "image");

    mat4 &camMatrix = cam->getMatrix();
    mat4 model_matrix = trans * rot * scale;

    glUniform1f(glGetUniformLocation(shader[SHADER::DEFAULT], "SkyR"), model.SkyR);

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
    glBindTexture(GL_TEXTURE_2D, Renderer::SM_depthTex);
    glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "shadowMap"), 1);
    glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "drawShadows"), 1);

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "depthBiasMVP"), 1, GL_FALSE, glm::value_ptr(Renderer::depthMVP));

    glUniform3f(glGetUniformLocation(shader[SHADER::DEFAULT], "lightPos"), light->getPos().x, light->getPos().y, light->getPos().z);
    glUniform3f(glGetUniformLocation(shader[SHADER::DEFAULT], "lightDir"), light->getDir().x, light->getDir().y, light->getDir().z);
    glUniform3fv(glGetUniformLocation(shader[SHADER::DEFAULT], "viewPos"), 1, &cam->pos[0]);	
    glUniform1f(glGetUniformLocation(shader[SHADER::DEFAULT], "intensity_factor"), 1.0); // full alpha
    glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "alphaTest"), alphaTest); // full alpha

    if (!reflects) {
        // Draw shape as a stencil
        glEnable(GL_STENCIL_TEST);
        stencil_bit = stencil_bit | Stencil::sil;
        glStencilFunc(GL_ALWAYS, stencil_bit, 0xFF); // Set any stencil to our sb value
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(stencil_bit); // Clear the current stencil bit
        glClear(GL_STENCIL_BUFFER_BIT);
    }

    glEnablei(GL_BLEND, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CheckGLErrors("loadUniforms in render");

    glDrawElements(
        GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        model.num_indices(), //How many indices
        GL_UNSIGNED_INT,	 //Type
        (void*)0			 //Offset
        );
    
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

    glStencilFunc(GL_NOTEQUAL, 1, Stencil::sil); // Pass test if stencil value is 1
    glStencilMask(Stencil::none); // Don't write anything to stencil buffer

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

// Uses the stencil buffer drawn to when drawing the track, so all cars/other reflectable items must appear
// after Track in the entity list
void Renderer::reflectCar(Car* car, glm::mat4 &perspectiveMatrix, float reflectivity) {
    RaycastResults raycast = car->doRaycast();
    if (raycast.distance <= 0) {
        return;
    }
    vec3 r_offset = (2.0f * -raycast.normal * raycast.distance);
    quat base = quat(vec3(0.0f, 0.0f, 0.0f));

    // Flip the car along its axis, calculate rotation
    quat flip = glm::rotate(base, (float)M_PI, car->getDir());
    quat model_rot = flip * car->getQRot(); // order is important!
    mat4 rot = glm::mat4_cast(model_rot);

    // Calculate translation
    vec3 car_reflect_pos = car->getPos() + r_offset;
    mat4 trans = glm::translate(mat4(), car_reflect_pos);

    // Test that we only draw onto track, and mark the new areas that we draw with refDrawn
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_EQUAL, Stencil::track | Stencil::refDrawn, Stencil::track); // MAX: set middle 'ref' to 'sil' as well? test after
    glStencilMask(Stencil::track | Stencil::refDrawn);
    
    // Draw the car
    for (Model* mod : car->getModels()) {
        drawShade(*mod, perspectiveMatrix, mod->get_scaling(), rot, trans, true);
    }

    for (int i = 0; i < Car::NUM_WHEELS; i++) {
        float WHEEL_MIRROR_OFFSET = 2.5f;

        Wheel* wheel = car->getWheel(i);
        Model* wheelMod = wheel->getModels().at(0);
        vec3 localPos = wheel->getPos() - car->getPos(); // wheel offset relative to chassis
        model_rot = wheel->getQRot();
        rot = glm::mat4_cast(model_rot);
        trans = glm::translate(mat4(), car_reflect_pos + localPos + (WHEEL_MIRROR_OFFSET * raycast.normal));
        drawShade(*wheelMod, perspectiveMatrix, wheelMod->get_scaling(), rot, trans, true);
    }

    // Render hook:
    Hook* hook = car->getHook();
    if (hook->getShot()) {
        RaycastResults hookCast = car->doHookRaycast();
        r_offset = (2.0f * -hookCast.normal * hookCast.distance);

        vec3 fakeHookPos = hook->getPos() + r_offset;
        trans = glm::translate(mat4(), fakeHookPos);
        Model* hookMod = hook->getModels().at(0);
        drawShade(*hookMod, perspectiveMatrix, hookMod->get_scaling(), rot, trans, true); // leave rot unchanged

        // Rotate by base_rot (.obj model-dependent), then by the angle around the rotation axis
        quat none = quat(vec3(0.0f, 0.0f, 0.0f));

        // HookChain:
        HookChain* chain = hook->chain.get();
        vec3 hookDir = normalize(fakeHookPos - car_reflect_pos);
        vec3 rot_axis = normalize(cross(vec3(0.0f, 0.0f, -1.0f), hookDir));

        float angle = acos(-hookDir.z);
        model_rot = glm::rotate(none, angle, rot_axis) * chain->getBaseRot();

        rot = glm::mat4_cast(model_rot);
        trans = glm::translate(mat4(), (fakeHookPos + car_reflect_pos) / 2.0f);
        Model* chainMod = chain->getModels().at(0);
        drawShade(*chainMod, perspectiveMatrix, chainMod->get_scaling(), rot, trans, true);
    }
}

void Renderer::reflectWalls(Walls* walls, glm::mat4 &perspectiveMatrix, float reflectivity) {
    vec3 wallPos = walls->getPos();
    wallPos.y -= walls->height;
    mat4 trans = glm::translate(mat4(), wallPos);
    mat4 rot = glm::mat4_cast(walls->getQRot()); // order is important!

    // Test that we only draw onto track, and mark the new areas that we draw with refDrawn
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_EQUAL, Stencil::track | Stencil::refDrawn, Stencil::track); // MAX: set middle 'ref' to 'sil' as well? test after
    glStencilMask(Stencil::track | Stencil::refDrawn);
    glDepthFunc(GL_GEQUAL);

    for (Model* mod : walls->getModels()) {
        drawShade(*mod, perspectiveMatrix, mod->get_scaling(), rot, trans, true, true);
    }
    glDepthFunc(GL_LESS);

}

// Draw the track in 2 steps: opaque if Stencil::refDrawn not set, translucent otherwise
void Renderer::drawTrack(const Model& model, glm::mat4 &perspectiveMatrix, glm::mat4 scale, glm::mat4 rot, glm::mat4 trans, float reflectivity) {
    glUseProgram(shader[SHADER::DEFAULT]);

    glBindVertexArray(model.vao[VAO::GEOMETRY]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->tex_id);
    glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "skybox"), 2);
    model.getTex()->load(GL_TEXTURE0, shader[SHADER::DEFAULT], "image");

    mat4 &camMatrix = cam->getMatrix();
    mat4 model_matrix = trans * rot * scale;

    glUniform1f(glGetUniformLocation(shader[SHADER::DEFAULT], "SkyR"), model.SkyR);

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
    glBindTexture(GL_TEXTURE_2D, Renderer::SM_depthTex);
    glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "shadowMap"), 1);

    glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "depthBiasMVP"), 1, GL_FALSE, glm::value_ptr(Renderer::depthMVP));

    glUniform3f(glGetUniformLocation(shader[SHADER::DEFAULT], "lightPos"), light->getPos().x, light->getPos().y, light->getPos().z);
    glUniform3f(glGetUniformLocation(shader[SHADER::DEFAULT], "lightDir"), light->getDir().x, light->getDir().y, light->getDir().z);
    glUniform3fv(glGetUniformLocation(shader[SHADER::DEFAULT], "viewPos"), 1, &cam->pos[0]);

    glEnablei(GL_BLEND, 0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    glUniform1f(glGetUniformLocation(shader[SHADER::DEFAULT], "intensity_factor"), 1.0); // draw opaque first
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, Stencil::refDrawn, Stencil::refDrawn); // Set any stencil to our sb value
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // no stencil writing
    glStencilMask(Stencil::refDrawn);

    glDrawElements(
        GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        model.num_indices(), //How many indices
        GL_UNSIGNED_INT,	 //Type
        (void*)0			 //Offset
    );


    glUniform1f(glGetUniformLocation(shader[SHADER::DEFAULT], "intensity_factor"), 1.0 - reflectivity); // draw translucent
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_EQUAL, Stencil::refDrawn, Stencil::refDrawn); // Set any stencil to our sb value
   // glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); // no stencil writing
   // glStencilMask(0xFF); // Overwrite anything

    glDrawElements(
        GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
        model.num_indices(), //How many indices
        GL_UNSIGNED_INT,	 //Type
        (void*)0			 //Offset
    );


    CheckGLErrors("drawShade");
    glDisablei(GL_BLEND, 0);
    glBindVertexArray(0);
}

void Renderer::drawScene(const std::vector<Renderable*>& ents, const std::vector<Renderable*>& cubes)
{
  //renderShadowMap(ents);
    
    // We could turn on the shadow map with a lower resolution ( scale it based on number of players)
    // This means that each car keeps track of their own shadows but they look worse the more players there are
    // This is not a long term solution but it could be good for a submission
    //renderShadowMap(ents);
    glViewport(vpX, vpY, width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right
    
    cam->update();
	//float fovy, float aspect, float zNear, float zFar
    mat4 perspectiveMatrix = cam->calcPerspective();
    glClearColor(0.f, 0.f, 0.f, 0.f);		//Color to clear the screen with (R, G, B, Alpha)
  //  glStencilMask(0x00);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);		//Clear stencil and depth buffers
    // clear color buffer bit too possibly
    glUseProgram(shader[SHADER::DEFAULT]);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glStencilFunc(GL_ALWAYS, 0, Stencil::clear); // Set any stencil to 0
    glClear(GL_STENCIL_BUFFER_BIT);

    Track* track = NULL;
    bool trackFound = false;
    Walls* walls = NULL;
    bool wallsFound = false;
    stencil_bit = Stencil::none;

    ///////////////////////////////////////
    //glm::mat4 depthBiasMVP = depthMVP;
    //glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "depthBiasMVP"), 1, GL_FALSE, &depthBiasMVP[0][0]);
    ///////////////////////////////////////

    if (index < 0) { // menu only
        glUseProgram(shader[SHADER::SKYBOX]);
        drawSkybox(perspectiveMatrix);
    }

    #pragma omp parallel for
    for (auto &it = ents.begin(); it < ents.end(); it++) {
        Renderable* r = *it;
        if (!r->canRender()) {
            continue;
        }
        int old_stencil = stencil_bit;
        // MAX: commented while debugging
         if (!trackFound) { // track pre-draw: modify stencil bit
            track = dynamic_cast<Track*>(r);
            if (track) {
                stencil_bit = stencil_bit | Stencil::track; // only track is reflective
                glStencilMask(stencil_bit); // Write to stencil buffer
                glClear(GL_STENCIL_BUFFER_BIT);
                alphaTest = true;

            }
        }
        else if (r->isCar()) {
            Car* car = static_cast<Car*>(r);
            reflectCar(car, perspectiveMatrix);
        }

		for (Model* model : r->getModels()) {
            mat4 scale = model->get_scaling();
            mat4 rot = glm::mat4_cast(r->getQRot());
            mat4 base_trans = glm::translate(mat4(), r->getPos());
            renderModel(*model, perspectiveMatrix, scale, rot, base_trans);
        }

        if (!trackFound && track) {
            trackFound = true;
            alphaTest = false;

            // Draw the skybox after filling the track stencil bit
            glClear(GL_DEPTH_BUFFER_BIT);
            glUseProgram(shader[SHADER::SKYBOX]);
            drawSkybox(perspectiveMatrix);
            glUseProgram(shader[SHADER::DEFAULT]);

        }

        stencil_bit = old_stencil;
    }

    // draw skyline
    drawSkyline(cubes);

    // draw track
   if (trackFound) {
        for (Model* model : track->getModels()) {
            mat4 scale = model->get_scaling();
            mat4 rot = glm::mat4_cast(track->getQRot());
            mat4 trans = glm::translate(mat4(), track->getPos());
            drawTrack(*model, perspectiveMatrix, scale, rot, trans, 0.2);
        }
    } else {
    //    std::cout << "Warning: not rendering track" << std::endl;
    }
}


void Renderer::drawText() {
    // Draw text here

//    (width*height) / (1280 * 720);
//    (height / 720.f)
    float size = 60 * (height / 720.f);
    int xPlacement = (size/2.f);//30
    int yPlacement = this->height - (size*1.66);//-100
    int ds_offset = 3;

    //Lap Placement - insert real lap information here

    char lapText[256];
    
    sprintf(lapText, "\n%i\\3",cam->getCar()->getLap());
    drawDropShadowText("LAP", blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    drawDropShadowText(lapText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);

    //Timer Text - insert real timer info here
    xPlacement = (size/2.f);//30
    yPlacement = (size*1.66f);//100
    char timeText[256];
   
    double currentTime = glfwGetTime();
   
    if (cam->getCar()->doneRace) {
        currentTime = cam->getCar()->raceTime;
    }

    sprintf(timeText, "\n%.2i:%.2i", int(currentTime)/60,int(currentTime)%60);
    drawDropShadowText("TIME", cam->getCar()->raceTime?redText:blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    drawDropShadowText(timeText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);

    size = 100 * (height / 720.f);
    //Position - insert real position info here
    xPlacement = this->width - (size*3.5);//300
    yPlacement = this->height - (size*1.5f);//150
    char posText[256];
    switch (cam->getCar()->rank)
    {
    case 1: sprintf(posText, "1ST");
        break;
    case 2: sprintf(posText, "2ND");
        break;
    case 3: sprintf(posText, "3RD");
        break;
    case 4: sprintf(posText, "4TH");
        break;

    }
    
    drawDropShadowText(posText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    size = 100 * (height / 720.f);
    
    xPlacement = this->width - (size*3);
    yPlacement = size;
    int speed_int = (int)cam->getCarSpeed();
    std::string speed_str = std::to_string(speed_int);
    drawDropShadowText(speed_str.c_str(), whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    size = 100 * (height / 720.f);
    if (cam->getCar()->doneRace) {
        if (cam->getCar()->raceTime == 0) {
            cam->getCar()->raceTime = glfwGetTime();
        }
        char winText[256];
        sprintf(winText, "FINISHED");

        xPlacement = (width /2) - ((size/2)* ((strlen(winText)/2.f)+1));
        yPlacement = height - 3*size;
        drawDropShadowText(winText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    if (cam->getCar()->cooldownState) {

        if((clock() - cam->getCar()->start) / double(CLOCKS_PER_SEC) > 2.f) {
            hookNotReady3->drawTexture((width / 2) - (size / 2.f), 0, size, size, width, height, 1, true);
        }
        else if((clock() - cam->getCar()->start)/ double(CLOCKS_PER_SEC) > 1.f) {
            hookNotReady2->drawTexture((width / 2) - (size / 2.f), 0, size, size, width, height, 1, true);
        }
        else if((clock() - cam->getCar()->start) / double(CLOCKS_PER_SEC) > 0.f) {
            hookNotReady1->drawTexture((width / 2) - (size / 2.f), 0, size, size, width, height, 1, true);
        }
    }
    else {
        hookReady->drawTexture((width / 2) - (size/2.f), 0, size, size, width, height, 1, true);
    }

}

void Renderer::drawBlackBarText(const char* string, Text2D* front, Text2D* back, int x, int y, int size, int offset, int sWidth, int sHeight) {
    blackBar->drawTexture(0, y, width, size, width, sHeight, 0.4);
   // back->printText2D(string, x, y, size, this->width, this->height);
    front->printText2D(string, x, y, size, sWidth, sHeight);
}

void Renderer::drawDropShadowText(const char* string, Text2D* front, Text2D* back, int x, int y, int size, int offset) {
    back->printText2D(string, x, y, size, this->width, this->height);
    front->printText2D(string, x + offset, y - offset, size, this->width, this->height);
}

void Renderer::drawCountDown(int time)
{
    //glViewport(0, 0, width, height);
    glViewport(vpX, vpY, width, height);
    glUseProgram(shader[SHADER::TEXT]);

    //glDisable(GL_CULL_FACE);
    int xPlacement;
    int yPlacement;
    int ds_offset = 3;
    int size = 200 * (height / (720.f));

    char text[512];
    if (time == 0) {
        sprintf(text, "GO");
    }
    else {
        sprintf(text, "%i", time);
    }
    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(text) / 2.f) - (size);
    yPlacement = (this->height / 2.f)*(2.f / 3.f);

    drawDropShadowText(text, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
}

void Renderer::drawScores(const std::vector<Car*>& cars, float screenWidth, float screenHeight, int numberOfPlayers, int time)
{
    //screenWidth = width;
    //screenHeight = height;
    //glViewport(0, 0, width, height);
    glViewport(0, 0, screenWidth, screenHeight);
    glUseProgram(shader[SHADER::TEXT]);

    //glDisable(GL_CULL_FACE);
    float xPlacement;
    float yPlacement;
    int ds_offset = 3;
    int size = 80 * (screenHeight / (720.f));

    char text[512];
    xPlacement = (screenWidth / 2.f) - (size / 2.f) * ((strlen(text) / 2.f)+0.5);
    yPlacement = screenHeight*0.15f;


    char complete[512];
    sprintf(complete, "RACE COMPLETE");

    //drawDropShadowText(complete, blueText, blackText, xPlacement , screenHeight* 0.85f, size, ds_offset);
    
    drawBlackBarText(complete, blueText, blackText, xPlacement, screenHeight* 0.85f, size, ds_offset, screenWidth, screenHeight);


    xPlacement = (screenWidth / 8.f) - (size / 2.f) * (strlen(text) / 2.f);

    for (Car * c : cars) {
        float currentTime = c->raceTime;
        if (c->isAI()) {
            sprintf(text, "%i. CPU %i     TIME: %.2i:%.2i", c->rank, (c->color + 1)- numberOfPlayers, int(currentTime) / 60, int(currentTime) % 60);
        }
        else {
            sprintf(text, "%i. PLAYER %i  TIME: %.2i:%.2i ", c->rank, c->color + 1, int(currentTime) / 60, int(currentTime) % 60);
        }
        drawBlackBarText(text, whiteText, blackText,0, screenHeight - ((c->rank - 0.5f) * yPlacement)-(1.5*yPlacement), size, ds_offset, screenWidth, screenHeight);
    }

    size = size = 40 * (screenHeight / (720.f));
    char timer[512];
    sprintf(timer, "RETURNING TO MENU IN:    %is", time);
    drawBlackBarText(timer, redText, blackText, 0, yPlacement, size, ds_offset, screenWidth, screenHeight);
    //drawDropShadowText(timer, redText, blackText, 0, yPlacement, size, ds_offset);
}

int Renderer::getMMSize()
{
    return mmSize;
}

int Renderer::getWidth()
{
    return width;
}

int Renderer::getHeight()
{
    return height;
}

void Renderer::setTrack(int selectedTrack)
{
    track = selectedTrack;
}



// This seems kinda dangerous
Camera* Renderer::getCam() {
    return cam;
}

void Renderer::setDims(int width, int height) {
    this->width = width;
    this->height = height;
    this->vpX = 0;
    this->vpY = 0;

    cam->setDims(width, height);
    if (index == 0) {
        mmSize = height / 2;
        glDeleteFramebuffers(1, &mm_frameBuffer);
        glDeleteTextures(1, &mm_tex);
        glDeleteFramebuffers(1, &mm_pips_frameBuffer);
        glDeleteTextures(1, &mm_pips_tex);
        initColorFrameBuffer(mm_frameBuffer, mm_tex, mmSize, mmSize);
        initColorFrameBuffer(mm_pips_frameBuffer, mm_pips_tex, mmSize, mmSize);
        mm_bg_drawn = false;
    }
    UIScale = height / 1000.0f;
}

void Renderer::setDims(renderWindowData& rwd) {
    this->width = rwd.width;
    this->height = rwd.height;
    this->vpX = rwd.xPos;
    this->vpY = rwd.yPos;

    cam->setDims(width, height);
    if (index == 0) {
        mmSize = height / 2;
        glDeleteFramebuffers(1, &mm_frameBuffer);
        glDeleteTextures(1, &mm_tex);
        glDeleteFramebuffers(1, &mm_pips_frameBuffer);
        glDeleteTextures(1, &mm_pips_tex);
        initColorFrameBuffer(mm_frameBuffer, mm_tex, mmSize, mmSize);
        initColorFrameBuffer(mm_pips_frameBuffer, mm_pips_tex, mmSize, mmSize);
        mm_bg_drawn = false;
    }
    UIScale = height / 1000.0f;
}

void Renderer::renderMiniMapBG(const std::vector<Renderable*>& ents, float height, int size, int xPos, int yPos, float sWidth, float sHeight, float alpha) {
    Camera * mapCam = new Camera(vec3(0.05, -0.9, 0.), vec3(-300, height, -124.5), false); // tuned to this specific map
    mapCam->setDims(size, size);
    glViewport(0, 0, size, size);
    glBindFramebuffer(GL_FRAMEBUFFER, mm_frameBuffer);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4 perspectiveMatrix = mapCam->calcPerspective();
    mat4 &camMatrix = mapCam->getMatrix();

    for (auto& e : ents) {
        // This is virtual function lookup for each entity, might be slow
        // Potentially optimize by using a single vec of Renderables
        if (Track* track = dynamic_cast<Track*>(e)) {
            Model* model = track->getModels().at(0);
            mat4 scale = model->get_scaling();
            mat4 rot = glm::mat4_cast(track->getQRot());
            mat4 track_trans = glm::translate(mat4(), track->getPos());
            mat4 mmatrix = track_trans * rot * scale;

            glUseProgram(shader[SHADER::SIL]);

            glBindVertexArray(model->vao[VAO::GEOMETRY]);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);
            glDrawBuffer(GL_FRONT_AND_BACK);

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

            glUniform3f(glGetUniformLocation(shader[SHADER::SIL], "u_color1"), 1.0, 1.0, 1.0);

            CheckGLErrors("loadUniforms");

            // Still using default framebuffer => silhouettes update default framebuffer
            glDrawElements(
                GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
                model->num_indices(), //How many indices
                GL_UNSIGNED_INT,	 //Type
                (void*)0			 //Offset
            );
            CheckGLErrors("drawSil");

           // screenshot("ss.tga", width, height);
            glBindVertexArray(0);
            break;
        }
    }
    mapCam->setDims(skyline_mm_size, skyline_mm_size);
    glViewport(0, 0, skyline_mm_size, skyline_mm_size);
    glBindFramebuffer(GL_FRAMEBUFFER, skyline_mm_buffer);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto& e : ents) {
        // This is virtual function lookup for each entity, might be slow
        // Potentially optimize by using a single vec of Renderables
        if (Track* track = dynamic_cast<Track*>(e)) {
            Model* model = track->getModels().at(0);
            mat4 scale = model->get_scaling();
            mat4 rot = glm::mat4_cast(track->getQRot());
            mat4 track_trans = glm::translate(mat4(), track->getPos());
            mat4 mmatrix = track_trans * rot * scale;

            glUseProgram(shader[SHADER::SIL]);

            glBindVertexArray(model->vao[VAO::GEOMETRY]);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_STENCIL_TEST);
            glDrawBuffer(GL_FRONT_AND_BACK);

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

            glUniform3f(glGetUniformLocation(shader[SHADER::SIL], "u_color1"), 1.0, 1.0, 1.0);

            CheckGLErrors("loadUniforms");

            // Still using default framebuffer => silhouettes update default framebuffer
            glDrawElements(
                GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
                model->num_indices(), //How many indices
                GL_UNSIGNED_INT,	 //Type
                (void*)0			 //Offset
            );
            CheckGLErrors("drawSil");

            // screenshot("ss.tga", width, height);
            glBindVertexArray(0);
            break;
        }
    }
  //  Texture mmTex(mm_tex);
  //  Text2D mmRenderer(&mmTex);
  //  mmRenderer.drawTexture(xPos, yPos, size, size, sWidth, sHeight, alpha, true);

    mm_bg_drawn = true;
}

void Renderer::renderMiniMap(const std::vector<Renderable*>& ents, const std::vector<Car*>& cars, float height, int size, int xPos, int yPos, float sWidth, float sHeight, float alpha) {
    Camera * mapCam = new Camera(vec3(0.05, -0.9, 0.), vec3(-300, height, -124.5), false); // tuned to this specific map
    mapCam->setDims(size, size);
    glViewport(0, 0, size, size);
   // glViewport(0, 0, this->width, this->height);
    if (!mm_bg_drawn) {
        renderMiniMapBG(ents, height, size, xPos, yPos, sWidth, sHeight, alpha);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, mm_pips_frameBuffer);
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);

    mat4 perspectiveMatrix = mapCam->calcPerspective();
    mat4 &camMatrix = mapCam->getMatrix();

    // render pips
    for (auto& c : cars) {
        Model* model = c->getModels().at(0);
        vec4 v = vec4(c->getPos(), 1.);
        mat4 &camMatrix = mapCam->getMatrix();
        mat4 perspectiveMatrix = mapCam->calcPerspective();
        mat4 scale = model->get_scaling();
        vec4 screen = perspectiveMatrix * camMatrix * v;
        screen.x = (screen.x * 0.5 / screen.w + 0.5) * size;
        screen.y = (screen.y * 0.5 / screen.w + 0.5) * size;
        float pipSize = size / 10; // seems like a decent approximation
        mmPips->printText2D(CarRenderInfo::getMinimapIndex(c->color), screen.x - pipSize, screen.y - pipSize / 2, pipSize, size, size);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
    delete mapCam;
    glViewport(0, 0, sWidth, sHeight);

    // At this point, minimap draw to texFB
    Texture mmTex(mm_tex);
    Text2D mmRenderer(&mmTex);
    mmRenderer.drawTexture(xPos, yPos, size, size, sWidth, sHeight, alpha, true);

    Texture mmTexPips(mm_pips_tex);
    Text2D mmPipsRenderer(&mmTexPips);
    mmPipsRenderer.drawTexture(xPos, yPos, size, size, sWidth, sHeight, alpha, true);
}

GLuint& Renderer::getMiniMapBG() {
    return skyline_mm_buffer;
}


// Draw everything on the screen screeen
void Renderer::draw(const std::vector<Renderable*>& ents, const std::vector<Car*>& cars, const std::vector<Renderable*>& cubes) {
    drawScene(ents, cubes);
    drawText();
    //renderMiniMap(ents, cars, 1300, mmSize, width - mmSize, height / 4, 0.7);
}

void Renderer::drawSkylineShadows(const std::vector<Renderable*>& cubes) {
    if (cubes.size() > 0) {
        glViewport(0, 0, SM_res, SM_res);
        glBindFramebuffer(GL_FRAMEBUFFER, SM_frameBuffer);

        Model* master = cubes.at(0)->getModels().at(0);
        mat4 scale = master->get_scaling();
        mat4 rot = glm::mat4_cast(cubes.at(0)->getQRot());
        mat4 rs = rot * scale;
        mat4 trans; // uninit

        // Make sure depth testing is enabled and cull front faces
        glEnable(GL_DEPTH_TEST);
        //glEnable(GL_CULL_FACE);
        //  glCullFace(GL_BACK);

        glUseProgram(shader[SHADER::SHADOW]);

        glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SHADOW], "depthMVP"), 1, false, &Renderer::depthMVP[0][0]);

        // Set object-specific VAO
        glBindVertexArray(master->vao[VAO::GEOMETRY]);

        #pragma omp parallel for
        for (auto &it = cubes.begin(); it < cubes.end(); it++) {
            Renderable* cube = *it;
            trans = glm::translate(mat4(), cube->getPos());
            mat4 model_matrix = trans * rs;

            glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SHADOW], "model"), 1, false, &model_matrix[0][0]);

            glDrawElements(
                GL_TRIANGLES,		    //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
                master->num_indices(),    //How many indices
                GL_UNSIGNED_INT,	    //Type
                (void*)0			    //Offset
            );
        }

        //Switch back to the original frame buffer and reset face culling to back face culling
        //glCullFace(GL_BACK);
        glBindVertexArray(0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void Renderer::drawSkyline(const std::vector<Renderable*>& cubes) {
    if (cubes.size() > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_STENCIL_BUFFER_BIT);
        Model* master = cubes.at(0)->getModels().at(0);
        mat4 &camMatrix = cam->getMatrix();
        mat4 scale = master->get_scaling();
        mat4 rot = glm::mat4_cast(cubes.at(0)->getQRot());
        mat4 rs = rot * scale;
        mat4 trans; // uninit
        mat4 perspectiveMatrix = cam->calcPerspective();

        // Default shading
        glUseProgram(shader[SHADER::DEFAULT]);

        glBindVertexArray(master->vao[VAO::GEOMETRY]);

        glEnable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->tex_id);
        glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "skybox"), 2);

        glUniform1f(glGetUniformLocation(shader[SHADER::DEFAULT], "SkyR"), master->SkyR);

        glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "cameraMatrix"),
            1,
            false,
            &camMatrix[0][0]);

        glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "perspectiveMatrix"),
            1,
            false,
            &perspectiveMatrix[0][0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, Renderer::SM_depthTex);
        glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "shadowMap"), 1);

        glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "depthBiasMVP"), 1, GL_FALSE, glm::value_ptr(Renderer::depthMVP));

        glUniform3f(glGetUniformLocation(shader[SHADER::DEFAULT], "lightPos"), light->getPos().x, light->getPos().y, light->getPos().z);
        glUniform3f(glGetUniformLocation(shader[SHADER::DEFAULT], "lightDir"), light->getDir().x, light->getDir().y, light->getDir().z);
        glUniform3fv(glGetUniformLocation(shader[SHADER::DEFAULT], "viewPos"), 1, &cam->pos[0]);
        glUniform1f(glGetUniformLocation(shader[SHADER::DEFAULT], "intensity_factor"), 1.0); // full alpha
        glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "alphaTest"), alphaTest); // full alpha

        if (index < 0) { // menuRenderer only
            glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "drawShadows"), 0);
        }
        else {
            glUniform1i(glGetUniformLocation(shader[SHADER::DEFAULT], "drawShadows"), 1);
        }

        glEnable(GL_STENCIL_TEST);
        stencil_bit = stencil_bit | Stencil::sil;
        glStencilFunc(GL_ALWAYS, stencil_bit, 0xFF); // Set any stencil to our sb value
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        glStencilMask(stencil_bit); // Clear the current stencil bit
        glClear(GL_STENCIL_BUFFER_BIT);

        glEnablei(GL_BLEND, 0);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        CheckGLErrors("loadUniforms in render");

        #pragma omp parallel for
        for (auto &it = cubes.begin(); it < cubes.end(); it++) {
            Renderable* cube = *it;
            trans = glm::translate(mat4(), cube->getPos());
            mat4 model_matrix = trans * rs;

            cube->getModels().at(0)->getTex()->load(GL_TEXTURE0, shader[SHADER::DEFAULT], "image");

            glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::DEFAULT], "modelviewMatrix"),
                1,
                false,
                &model_matrix[0][0]);

            glDrawElements(
                GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
                master->num_indices(), //How many indices
                GL_UNSIGNED_INT,	 //Type
                (void*)0			 //Offset
            );
        }

        CheckGLErrors("drawShade");
        glDisablei(GL_BLEND, 0);
        glBindVertexArray(0);

        // Silling
        scale = scale * glm::scale(glm::vec3(master->sil_x, master->sil_y, master->sil_z));
        rs = rot * scale;
        glUseProgram(shader[SHADER::SIL]);

        glBindVertexArray(master->vao[VAO::GEOMETRY]);

        glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SIL], "cameraMatrix"),
            1,
            false,
            &camMatrix[0][0]);

        glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SIL], "perspectiveMatrix"),
            1,
            false,
            &perspectiveMatrix[0][0]);

        CheckGLErrors("loadUniforms");

        glDepthMask(GL_TRUE); // enable writes to Z-buffer

        glUniform3f(glGetUniformLocation(shader[SHADER::SIL], "u_color1"), 0.0, 0.0, 0.0);

        glStencilFunc(GL_NOTEQUAL, 1, Stencil::sil); // Pass test if stencil value is 1
        glStencilMask(Stencil::none); // Don't write anything to stencil buffer

        // Still using default framebuffer => silhouettes update default framebuffer

        #pragma omp parallel for
        for (auto &it = cubes.begin(); it < cubes.end(); it++) {
            Renderable* cube = *it;
            trans = glm::translate(mat4(), cube->getPos());
            mat4 model_matrix = trans * rs;
            glUniformMatrix4fv(glGetUniformLocation(shader[SHADER::SIL], "modelviewMatrix"),
                1,
                false,
                &model_matrix[0][0]);

            glDrawElements(
                GL_TRIANGLES,		 //What shape we're drawing	- GL_TRIANGLES, GL_LINES, GL_POINTS, GL_QUADS, GL_TRIANGLE_STRIP
                master->num_indices(), //How many indices
                GL_UNSIGNED_INT,	 //Type
                (void*)0			 //Offset
            );
        }

        CheckGLErrors("drawSil");
        glBindVertexArray(0);
    }
}


void Renderer::drawNoText(const std::vector<Renderable*>& ents, const std::vector<Renderable*>& cubes) {
    drawScene(ents, cubes);
}