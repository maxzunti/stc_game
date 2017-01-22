#include <iostream>
#include "renderer.h"
using namespace std;

int main()
{
	Renderer* myRenderer = new Renderer(0);
	// Input* myInput = new Input();
	// Physics* myPhysics = new Physics();
	// Sound* mySound = new Sound();
	// OpponentAI* myAI = new OpponentAI();

	myRenderer->initRenderer();

	while (!myRenderer->shouldClose())
	{
		// myInput->getState();
		// myAI->getState();
		// myPhysics->simulate();
		// mySound->updateSound();
		
		myRenderer->drawScene();
	}

	return 0;
}