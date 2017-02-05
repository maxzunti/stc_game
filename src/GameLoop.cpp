#include <iostream>

#include "renderer/Window.h"
#include "renderer\GLUtil.h"

#include "input/input.h"
#include "entity/Entity.h"
#include "entity/Renderable.h"

using namespace std;

int main(int argc, const char* argv[])
{
	std::cout << argv[0] << std::endl;
    Window * myWindow = new Window();

	//exit(0);
//	Renderer* myRenderer = new Renderer(0);
	// Input* myInput = new Input();
	// Physics* myPhysics = new Physics();
	// Sound* mySound = new Sound();
	// OpponentAI* myAI = new OpponentAI();

//	myRenderer->initRenderer();
	Input *input = new Input(0);

    // TODO: convert these to unique_ptrs
    std::vector<Entity*> entities(0);
    //entities.push_back(new Renderable("assets/models/teapot/teapot.obj", "assets/models/teapot/teapot_tex.png"));
    Renderable *teapot = new Renderable("assets/models/teapot/teapot.obj", "assets/models/teapot/teapot_tex.png");
    teapot->scale(0.5, 0.5, 0.5);
    entities.push_back(teapot);
    entities.push_back(new Renderable("assets/models/plane/plane.obj", "assets/models/plane/logo_tile.png"));

	while (!myWindow->shouldClose())
	{
		// myInput->getState();
		// myAI->getState();
		// myPhysics->simulate();
		// mySound->updateSound();

		myWindow->draw(entities);
		input->Update();
		std::cout << "LS_X: " << input->LeftStick_X() << "  LS_Y: " << input->LeftStick_Y() << "  RS_X: " << input->RightStick_X() << "  RS_Y: " << input->RightStick_Y() << std::endl;
	}

	delete myWindow;
    delete input;
	return 0;
}