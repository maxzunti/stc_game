#pragma once
using namespace std;

class Renderer {
	int index;
public:
	Renderer(int);
	~Renderer();
	bool initRenderer();
	void drawScene();
	bool shouldClose();
};
