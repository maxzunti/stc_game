#pragma once
#include "util/GLUtil.h"
#include "util/Model.h"

class Renderer {
	int index;
public:
	Renderer(int);
	~Renderer();
	bool initRenderer();
	void drawScene();
	bool shouldClose();
};

// These really don't belong in renderer, but we should remove them eventually anyway
void generateTorus(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals,
	std::vector<glm::vec2>& uvs, std::vector<unsigned int>& indices,
	float c_r, float t_r,
	int uDivisions, int vDivisions);
void generatePSphere(std::vector<glm::vec3>& positions, std::vector<glm::vec3>& normals,
	std::vector<glm::vec2>& uvs, std::vector<unsigned int>& indices,
	float r, glm::vec3 center,
	int uDivisions, int vDivisions);