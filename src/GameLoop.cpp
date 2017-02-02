#include <iostream>
#include "renderer/renderer.h"
#include "input/input.h"
#include "entity/Entity.h"
#include "entity/Renderable.h"

using namespace std;

int main(int argc, const char* argv[])
{
	std::cout << argv[0] << std::endl;
	//exit(0);
	Renderer* myRenderer = new Renderer(0);
	// Input* myInput = new Input();
	// Physics* myPhysics = new Physics();
	// Sound* mySound = new Sound();
	// OpponentAI* myAI = new OpponentAI();

	myRenderer->initRenderer();
	Input *input = new Input(0);

	while (!myRenderer->shouldClose())
	{
		// myInput->getState();
		// myAI->getState();
		// myPhysics->simulate();
		// mySound->updateSound();
		
		myRenderer->drawScene();
		input->Update();
		std::cout << "LS_X: " << input->LeftStick_X() << "  LS_Y: " << input->LeftStick_Y() << "  RS_X: " << input->RightStick_X() << "  RS_Y: " << input->RightStick_Y() << std::endl;
	}

	delete myRenderer;

	return 0;
}