#pragma once
#include "GLUtil.h"
#include "model/Model.h"
#include "camera.h"
#include "../entity/Renderable.h"
#include <memory>

class Renderer {
	int index;
    Camera *cam; // 1 cam per renderer

    void render(const Model& model, glm::mat4 perspectiveMatrix, int startElement);

public:
	Renderer(int);
	~Renderer();
	bool initRenderer();
	void drawScene(const std::vector<Entity*>& ents);

    Camera* getCam();
};