#pragma once
#include "GLUtil.h"
#include "model/Model.h"
#include "skybox/Skybox.h"
#include "camera.h"
#include "../entity/Renderable.h"
#include "../input/input.h"
#include <memory>

class Renderer {
	int index;
    Camera * cam; // 1 cam per renderer
    Skybox * skybox;
    Input * controller;
    const enum camMode { FREE = 0, FOLLOW };
    camMode mode;

    void render(const Model& model, glm::mat4 &perspectiveMatrix, glm::mat4 model_matrix, int startElement);
    void drawSkybox(const Skybox* sb, glm::mat4 &perspectiveMatrix);
    void updateCamera();

public:
    const float XBOX_X_CAM_ROT_SPEED = 0.05;
    const float XBOX_Y_CAM_ROT_SPEED = 0.05;

    const float XBOX_X_CAM_MOVE_SPEED = 0.2;
    const float XBOX_Y_CAM_MOVE_SPEED = 0.2;
    const float XBOX_Z_CAM_MOVE_SPEED = 0.2;

	Renderer(int);
	~Renderer();
    void initSkybox();
	void drawScene(const std::vector<Entity*>& ents);
    void registerController(Input *);

    Camera* getCam();
};