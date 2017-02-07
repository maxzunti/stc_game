#include <iostream>

#include "renderer/Window.h"
#include "renderer\GLUtil.h"

#include "input/input.h"
#include "physics/PhysicsManager.h"
#include "entity/Entity.h"
#include "entity/Renderable.h"
#include "entity/PhysicsObject.h"
#include "renderer/skybox/Skybox.h"

using namespace std;

int main(int argc, const char* argv[])
{
    std::unique_ptr<Window> window(new Window());

    // Set up input
    std::unique_ptr<Input> input(new Input(0));
    window->getRenderer()->registerController(input.get());


	// Input* myInput = new Input();
	PhysicsManager* myPhysics = new PhysicsManager();
	// Sound* mySound = new Sound();
	// OpponentAI* myAI = new OpponentAI();


    // TODO: convert these to unique_ptrs
    std::vector<Entity*> entities(0);
    PhysicsObject * tCrate = new PhysicsObject("assets/models/Crate/Crate1.obj", "assets/models/Crate/crate_1.jpg", myPhysics->createBlock());
    entities.push_back(tCrate);
    tCrate->setPos(0, 0, -5);
   // tCrate->setRot(0, 0, 1.56);

    // Teapot test obj
    //Renderable *teapot = new Renderable("assets/models/teapot/teapot.obj", "assets/models/teapot/teapot_tex.png");
    //teapot->scale(0.5, 0.5, 0.5);
    //entities.push_back(teapot);

    Renderable* plane = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/logo_tile.png");

    plane->setPos(0, -2, 0);
    entities.push_back(plane);

    myPhysics->createGroundPlane();
	
	while (!window->shouldClose())
	{
        input->Update();

		// myInput->getState();
		// myAI->getState();
		myPhysics->stepPhysics();
		
		// FYI: dynamic casts have a lot of run-time checking involved and are pretty expensive
        // so we should avoid using them as much as possible
	/*	PxRigidBodyExt::addForceAtLocalPos(*dynamic_cast<PhysicsObject*>(entities.front())->mActor, PxVec3(0, 160, 0), PxVec3(-0.9, -0.9, 0.9));
		PxRigidBodyExt::addForceAtLocalPos(*dynamic_cast<PhysicsObject*>(entities.front())->mActor, PxVec3(0, 160, 0), PxVec3(-0.9, -0.9, -0.9));
		PxRigidBodyExt::addForceAtLocalPos(*dynamic_cast<PhysicsObject*>(entities.front())->mActor, PxVec3(0, 160, 0), PxVec3(0.9, -0.9, -0.9));
		PxRigidBodyExt::addForceAtLocalPos(*dynamic_cast<PhysicsObject*>(entities.front())->mActor, PxVec3(0, 160, 0), PxVec3(0.9, -0.9, 0.9)); */
        //PxRigidBodyExt::addForceAtLocalPos(*dynamic_cast<PhysicsObject*>(entities.front())->mActor, PxVec3(0, 640, 0), PxVec3(0, 0, 0));
        PxRigidBodyExt::addForceAtLocalPos(*tCrate->mActor, PxVec3(0, 640, 0), PxVec3(0, 0, 0));
		
		//Watch here when you run. Applying a torque about Y (up) axis and it doesn't spin about it properly... why?
		tCrate->mActor->addTorque(PxVec3(0., -50, 0.)); // mad torques tho

        //dynamic_cast<PhysicsObject*>(entities.front())->mActor->addTorque(PxVec3(0., 0., 100.0));


		dynamic_cast<PhysicsObject*>(entities.front())->updatePosandRot(); // We'll eventually have a function here that updates all positions
		// mySound->updateSound();
		window->draw(entities);
		//std::cout << "LS_X: " << input->LeftStick_X() << "  LS_Y: " << input->LeftStick_Y() << "  RS_X: " << input->RightStick_X() << "  RS_Y: " << input->RightStick_Y() << std::endl;
	}
	
	delete myPhysics;
	return 0;
}