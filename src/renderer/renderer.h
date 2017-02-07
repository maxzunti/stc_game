#pragma once
#include "GLUtil.h"
#include "model/Model.h"
#include "skybox/Skybox.h"
#include "camera.h"
#include "../entity/Renderable.h"
#include <memory>

class Renderer {
	int index;
    Camera *cam; // 1 cam per renderer
    Skybox * skybox;

    void render(const Model& model, glm::mat4 &perspectiveMatrix, glm::mat4 model_matrix, int startElement);
    void drawSkybox(const Skybox* sb, glm::mat4 &perspectiveMatrix);

public:
	Renderer(int);
	~Renderer();
    void initSkybox();
	void drawScene(const std::vector<Entity*>& ents);

    Camera* getCam();
};