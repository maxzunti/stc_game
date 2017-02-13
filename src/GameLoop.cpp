#include <iostream>

#include "renderer/Window.h"
#include "renderer\GLUtil.h"

#include "input/input.h"
#include "physics/PhysicsManager.h"
#include "entity/Entity.h"
#include "entity/Renderable.h"
#include "entity/PhysicsObject.h"
#include "entity/ProtoCar.h"
#include "entity/Hook.h"
#include "entity/Obstacle.h"
#include "physics/StickListener.h"
#include "renderer/skybox/Skybox.h"
#include <ctime>

#include "util/ConfigParser.h"

using namespace std;

int main(int argc, const char* argv[])
{

    std::unique_ptr<Window> window(new Window());

    // Set up input
    std::unique_ptr<Input> input(new Input(0));
  //  std::unique_ptr<Input> input2(new Input(1));


	// Input* myInput = new Input();
    StickListener stickListener;
	PhysicsManager* myPhysics = new PhysicsManager(&stickListener);
	// Sound* mySound = new Sound();
	// OpponentAI* myAI = new OpponentAI();

    // TODO: convert these to unique_ptrs
    std::vector<Entity*> entities(0);
    std::vector<ProtoCar*> cars(0);

    //ProtoCar * car = new ProtoCar("assets/models/Crate/Crate1.obj", "assets/models/Crate/crate_1.jpg", nullptr, myPhysics, input.get(), entities);
    ProtoCar *car = new ProtoCar("assets/models/Crate/Crate1.obj", "assets/models/Crate/crate_1.jpg", nullptr, myPhysics, input.get(), entities);
	//ProtoCar *dumCar = new ProtoCar("assets/models/Crate/Crate1.obj", "assets/models/Crate/crate_1.jpg", nullptr, myPhysics, input.get(), entities);
    window->getRenderer()->getCam()->registerController(input.get());
    window->getRenderer()->getCam()->registerCar(car);
   // ProtoCar * crate2 = new ProtoCar("assets/models/Crate/Crate1.obj", "assets/models/Crate/crate_1.jpg", myPhysics->createBlock(), input2.get());
    cars.push_back(car);
	//cars.push_back(dumCar);
    entities.push_back(car);
	//entities.push_back(dumCar);
//    entities.push_back(crate2);
    car->setPos(0, 7, 200);
	//dumCar->setPos(5, 7, 5);

	Obstacle * daCube = new Obstacle("assets/models/Crate/Crate1.obj", "assets/textures/cfa.png", myPhysics->createBlock(0.f, 100.f, 0.f, 100.f, 100.f, 100.f), myPhysics);
	entities.push_back(daCube);

    //Hook * tHook = new Hook("assets/models/Crate/Crate1.obj", "assets/models/teapot/teapot_tex.png", myPhysics->createBlock(0, 10, 0), myPhysics);
    //entities.push_back(tHook);

   // tCrate->setRot(0, 3.14 / 4., 0);
   //  tCrate->setRot(glm::vec3(0, 3.14/2., 0));

    // Teapot test obj
    //Renderable *teapot = new Renderable("assets/models/teapot/teapot.obj", "assets/models/teapot/teapot_tex.png");
    //teapot->scale(50, 50, 50);
	//teapot->setPos(0, 15, 0);
	//entities.push_back(teapot);


    Renderable* plane = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/logo_tile.png");
	Renderable* wall1 = new Renderable("assets/models/plane/plane.obj", "assets/textures/stars_milkyway.jpg");
	Renderable* wall2 = new Renderable("assets/models/plane/plane.obj", "assets/textures/texture_earth_surface.jpg");
	Renderable* wall3 = new Renderable("assets/models/plane/plane.obj", "assets/textures/texture_moon.jpg");
	Renderable* wall4 = new Renderable("assets/models/plane/plane.obj", "assets/textures/texture_sun.jpg");


    plane->setPos(0, 0, 0);
    plane->scale(50, 50, 50);

	//milky
	wall1->setPos(0, 5, -500);
	wall1->setRot(1.57, 0, 0);
    wall1->scale(50, 50, 50);

	//earth
	wall2->setPos(0, 5, 500);
	wall2->setRot(1.57, 0, 0);
	wall2->scale(50, 50, 50);

	//moon
	wall3->setPos(500, 5, 0);
	wall3->setRot(1.57, 1.57, 0);
	wall3->scale(50, 50, 50);

	//sun
	wall4->setPos(-500, 5, 0);
	wall4->setRot(1.57, 1.57, 0);
	wall4->scale(50, 50, 50);

    entities.push_back(plane);
	entities.push_back(wall1);
	entities.push_back(wall2);
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
   
		// FYI: dynamic casts have a lot of run-time checking involved and are pretty expensive
        // so we should avoid using them as much as possible
	/*	PxRigidBodyExt::addForceAtLocalPos(*dynamic_cast<PhysicsObject*>(entities.front())->mActor, PxVec3(0, 160, 0), PxVec3(-0.9, -0.9, 0.9));
		PxRigidBodyExt::addForceAtLocalPos(*dynamic_cast<PhysicsObject*>(entities.front())->mActor, PxVec3(0, 160, 0), PxVec3(-0.9, -0.9, -0.9));
		PxRigidBodyExt::addForceAtLocalPos(*dynamic_cast<PhysicsObject*>(entities.front())->mActor, PxVec3(0, 160, 0), PxVec3(0.9, -0.9, -0.9));
		PxRigidBodyExt::addForceAtLocalPos(*dynamic_cast<PhysicsObject*>(entities.front())->mActor, PxVec3(0, 160, 0), PxVec3(0.9, -0.9, 0.9)); */
        //PxRigidBodyExt::addForceAtLocalPos(*dynamic_cast<PhysicsObject*>(entities.front())->mActor, PxVec3(0, 640, 0), PxVec3(0, 0, 0));
		
		//Watch here when you run. Applying a torque about Y (up) axis and it doesn't spin about it properly... why?
		//tCrate->mActor->addTorque(PxVec3(0., -50, 0.)); // mad torques tho

        //dynamic_cast<PhysicsObject*>(entities.front())->mActor->addTorque(PxVec3(0., 0., 100.0));
      //  tCrate->applyLocalForce(0, 0, 500);
        //tCrate->rotate(0.0, 0.01, 0.0);
        car->update();

        //crate2->update();
		//tHook->update();
		// mySound->updateSound();
		window->draw(entities);
       // std::cout << "rot x = " << tCrate->xRot() << "  y_rot = " << tCrate->yRot() << "  z_rot = " << tCrate->zRot() << std::endl;
		// std::cout << "LS_X: " << input->LeftStick_X() << "  LS_Y: " << input->LeftStick_Y() << "  RS_X: " << input->RightStick_X() << "  RS_Y: " << input->RightStick_Y() << std::endl;
       // std::cout << "RT = " << input->RightTrigger() << "    LT = " << input->LeftTrigger() << std::endl;
	}
	
	delete myPhysics;
	return 0;
}