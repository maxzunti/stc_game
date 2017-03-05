#define SDL_MAIN_HANDLED

#include <iostream>

#include "renderer/Window.h"
#include "renderer\GLUtil.h"

#include "input/input.h"
#include "physics/PhysicsManager.h"
#include "entity/Entity.h"
#include "entity/Renderable.h"
#include "entity/PhysicsObject.h"
#include "entity/StaticPhysicsObject.h"
#include "entity/ProtoCar.h"
#include "entity/Hook.h"
#include "entity/Obstacle.h"
#include "physics/StickListener.h"
#include "renderer/skybox/Skybox.h"
#include "Jukebox.h"
#include <ctime>
#include "util/ConfigParser.h"

using namespace std;

int main(int argc, const char* argv[])
{
    std::unique_ptr<Window> window(new Window(1024, 768));

	//Music
	Jukebox *bgm = new Jukebox("assets/sound/shootingstars.mp3");
	bgm->play();

    // Set up input
    std::unique_ptr<Input> input(new Input(0));

	// Input* myInput = new Input();
    StickListener stickListener;
	PhysicsManager* myPhysics = new PhysicsManager(&stickListener);
	// OpponentAI* myAI = new OpponentAI();

    // TODO: convert these to unique_ptrs
    std::vector<Entity*> entities(0);
    std::vector<ProtoCar*> cars(0);

    ProtoCar *car = new ProtoCar("assets/models/car/testcar.obj", "assets/models/car/testcar_s1.png", nullptr, myPhysics, input.get(), entities);

    window->getRenderer()->getCam()->registerController(input.get());
    window->getRenderer()->getCam()->registerCar(car);
 
    cars.push_back(car);
	
    entities.push_back(car);

	Obstacle * daCube = new Obstacle("assets/models/Crate/Crate1.obj", "assets/models/plane/logo_tile.png", myPhysics->createBlock(0.f, 100.f, 0.f, 100.f, 100.f, 100.f), glm::vec3(100,100,100), myPhysics);
   // Obstacle * daPot = new Obstacle("assets/models/teapot/teapot.obj", "assets/models/teapot/teapot_tex.png", myPhysics->createBlock(-50.f, 15.f, 210.f, 4.f, 3.f, 4.f), glm::vec3(1,1,1), myPhysics);
    //Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
    
    StaticPhysicsObject * daPot = new StaticPhysicsObject("assets/models/teapot/teapot.obj", "assets/models/teapot/teapot_tex.png", myPhysics);
    entities.push_back(daCube);
    entities.push_back(daPot);
    daCube->setRot(0.0, 0.0, 3.14f);
    daPot->setPos(-50.f, 5.f, 210.f);

    Renderable* plane = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/logo_tile.png");
	Renderable* wall1 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/stc.png");
	Renderable* wall2 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/stc.png");
	Renderable* wall3 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/hearthstone.png");
	Renderable* wall4 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/hearthstone.png");

    plane->setPos(0, 0, 0);
    plane->scale(50, 50, 50);

	//milky
	wall1->setPos(0, -20, -500);
	wall1->setRot(1.57, 0, 0);
    wall1->scale(50, 50, 50);

	//stc
	wall2->setPos(0, -20, 500);
	wall2->setRot(-1.57, 0, -3.14);
	wall2->scale(50, 50, 50);

	//moon
	wall3->setPos(500, -20, 0);
	wall3->setRot(1.57, 1.57, 0);
	wall3->scale(50, 50, 50);

	//sun
	wall4->setPos(-500, -20, 0);
	wall4->setRot(1.57, 1.57, 0);
	wall4->scale(50, 50, 50);


    entities.push_back(plane);
//	entities.push_back(wall1);
	entities.push_back(wall2); //back
	entities.push_back(wall3);
	entities.push_back(wall4);


    //myPhysics->createGroundPlane();
    myPhysics->mScene->addActor(*createDrivablePlane(myPhysics->mMaterial, myPhysics->mPhysics));
	myPhysics->createWallPlane(0,5,-500,0,1);
	myPhysics->createWallPlane(0, 5, 500, 0, -1);
	myPhysics->createWallPlane(500, 5, 0, -1, 0);
	myPhysics->createWallPlane(-500, 5,  0, 1, 0);


    while (!window->shouldClose())
	{
        input->Update();
        
    // myInput->getState();
    // myAI->getState();

        for (const auto& c : cars)
            c->stepForPhysics();
		myPhysics->stepPhysics();
   
        car->update();
        daCube->update();
        daPot->update();
       
	// mySound->updateSound();
		window->draw(entities);
	}

	delete myPhysics;
	return 0;
}