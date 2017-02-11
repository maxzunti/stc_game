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
#include "renderer/skybox/Skybox.h"

using namespace std;

int main(int argc, const char* argv[])
{
    std::unique_ptr<Window> window(new Window());

    // Set up input
    std::unique_ptr<Input> input(new Input(0));
  //  std::unique_ptr<Input> input2(new Input(1));


	// Input* myInput = new Input();
	PhysicsManager* myPhysics = new PhysicsManager();
	// Sound* mySound = new Sound();
	// OpponentAI* myAI = new OpponentAI();


    // TODO: convert these to unique_ptrs
    std::vector<Entity*> entities(0);
    std::vector<ProtoCar*> cars(0);

    ProtoCar * car = new ProtoCar("assets/models/Crate/Crate1.obj", "assets/models/Crate/crate_1.jpg", nullptr, myPhysics, input.get(), entities);

    window->getRenderer()->getCam()->registerController(input.get());
    window->getRenderer()->getCam()->registerCar(car);
   // ProtoCar * crate2 = new ProtoCar("assets/models/Crate/Crate1.obj", "assets/models/Crate/crate_1.jpg", myPhysics->createBlock(), input2.get());
    cars.push_back(car);
    entities.push_back(car);
//    entities.push_back(crate2);
    car->setPos(0, 7, 0);

    Hook * tHook = new Hook("assets/models/Crate/Crate1.obj", "assets/models/teapot/teapot_tex.png", myPhysics->createBlock(0, 10, 0), myPhysics, 0.f);
    entities.push_back(tHook);

   // tCrate->setRot(0, 3.14 / 4., 0);
   //  tCrate->setRot(glm::vec3(0, 3.14/2., 0));




    // Teapot test obj
    //Renderable *teapot = new Renderable("assets/models/teapot/teapot.obj", "assets/models/teapot/teapot_tex.png");
    //teapot->scale(0.5, 0.5, 0.5);
    //entities.push_back(teapot);


    Renderable* plane = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/logo_tile.png");
	Renderable* wall = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/logo_tile.png");

    plane->setPos(0, 0, 0);
    plane->scale(50, 50, 50);

	wall->setPos(0, 5, -10);
	wall->setRot(1.57, 0, 0);

    entities.push_back(plane);
	entities.push_back(wall);

    myPhysics->createGroundPlane();
    //myPhysics->mScene->addActor(*createDrivablePlane(myPhysics->mMaterial, myPhysics->mPhysics));
	myPhysics->createWallPlane(0,5,-10,0,1);

	while (!window->shouldClose())
	{
   //     tCrate->setRot(0, 0.745, 0);
    //    tCrate->setRot(0.785, 0., 0.785);
      //  tCrate->setRot(0.785, 0.785, 0.0);
      //  tCrate->setRot(0.0, 0.785, 0.0);



   //     tCrate->setPos(0, 2, 0);

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
        PxRigidBodyExt::addForceAtLocalPos(*tHook->mActor, PxVec3(0, 0, -500), PxVec3(0, 0, 0));
		
		//Watch here when you run. Applying a torque about Y (up) axis and it doesn't spin about it properly... why?
		//tCrate->mActor->addTorque(PxVec3(0., -50, 0.)); // mad torques tho

        //dynamic_cast<PhysicsObject*>(entities.front())->mActor->addTorque(PxVec3(0., 0., 100.0));
      //  tCrate->applyLocalForce(0, 0, 500);
        //tCrate->rotate(0.0, 0.01, 0.0);
        car->update();

        //crate2->update();
		tHook->update();
		// mySound->updateSound();
		window->draw(entities);
       // std::cout << "rot x = " << tCrate->xRot() << "  y_rot = " << tCrate->yRot() << "  z_rot = " << tCrate->zRot() << std::endl;
		// std::cout << "LS_X: " << input->LeftStick_X() << "  LS_Y: " << input->LeftStick_Y() << "  RS_X: " << input->RightStick_X() << "  RS_Y: " << input->RightStick_Y() << std::endl;
       // std::cout << "RT = " << input->RightTrigger() << "    LT = " << input->LeftTrigger() << std::endl;
	}
	
	delete myPhysics;
	return 0;
}