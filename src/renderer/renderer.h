#pragma once
#include "GLUtil.h"
#include "model/Model.h"
#include "skybox/Skybox.h"
#include "camera.h"
#include "../input/input.h"
#include <memory>
#include "../entity/Light.h"

class Renderer {
	int index;
    Camera * cam; // 1 cam per renderer
    Skybox * skybox;

    // Light might need to be elsewhere
    Light * light;

    void render(const Model& model, glm::mat4 &perspectiveMatrix, glm::mat4 model_matrix, int startElement);
    void drawSkybox(const Skybox* sb, glm::mat4 &perspectiveMatrix);

public:
	Renderer(int);
	~Renderer();
    void initSkybox();
	void drawScene(const std::vector<Entity*>& ents);

    // The following are all required for shadow mapping 
    // We need to decide of we want to move them to a seperate location or not 
    GLuint FramebufferName = 1;
    GLuint depthTexture;
    glm::mat4 depthMVP;
    bool loadFrameBuffers();
    void renderShadowMap(const std::vector<Entity*>& ents);
    void shrender(const Model& model, glm::mat4 model_matrix, int startElement);


    Camera* getCam();
};