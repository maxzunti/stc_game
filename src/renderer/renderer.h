#pragma once
#include "GLUtil.h"
#include "model/Model.h"
#include "skybox/Skybox.h"
#include "camera.h"
#include "../input/input.h"
#include <memory>
#include "../entity/Light.h"
#include "model\Texture.h"
#include "text2D.h"

class Renderer {
	int index;
    Camera * cam; // 1 cam per renderer
    Skybox * skybox;

    // Light might need to be elsewhere
    Light * light;

    void initSkybox();
    void drawSkybox(glm::mat4 &perspectiveMatrix);

    // Control model-rendering flow
    void renderModel(const Model& model, glm::mat4 &perspectiveMatrix, glm::mat4 scale, glm::mat4 rot, glm::mat4 trans);

    // Render black-border silhouettes
    void drawSil(const Model& model, glm::mat4 &perspectiveMatrix, glm::mat4 &mmatrix);

    // Actually draw each model using standard shading
    void drawShade(const Model& model, glm::mat4 &perspectiveMatrix, glm::mat4 scale, glm::mat4 rot, glm::mat4 trans);

    // Shadow mapping functions
    void renderShadowMap(const std::vector<Entity*>& ents);
    void addToShadowMap(const Model& model, glm::mat4 model_matrix, int startElement);

    // The following are all required for shadow mapping 
    // We need to decide of we want to move them to a seperate location or not 
    GLuint SM_frameBuffer;
    GLuint SM_depthTex;
    glm::mat4 depthMVP;

    //Text rendering
    void Renderer::initText();
    Text2D * blueText;
    Text2D * blackText;
    Text2D * redText;
    Text2D * whiteText;
    
    // Init a frame buffer for a depth map, used for shadow mapping + silhouetting
    bool initDepthFrameBuffer(GLuint &frameBuffer, GLuint &depthTex, int width, int height);
    bool initColorFrameBuffer(GLuint &frameBuffer, GLuint &colTex, int width, int height);

    int width;
    int height;
    float SM_res = 8192;
    void drawText();
    void drawDropShadowText(const char* string, Text2D* front, Text2D* back, int x, int y, int size, int offset);
    int frameCount = 0;

public:
	Renderer(int);
	~Renderer();
    void postGLInit(); // call init functions after Window's created the OpenGL env.
	void drawScene(const std::vector<Entity*>& ents);
    void setDims(int width, int height);

    Camera* getCam();
};