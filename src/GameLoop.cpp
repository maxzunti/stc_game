#define SDL_MAIN_HANDLED

#include <iostream>

#include "renderer/Window.h"
#include "renderer/GLUtil.h"
#include "renderer/skybox/Skybox.h"

#include "physics/StickListener.h"
#include "physics/TriggerListener.h"

#include "input/input.h"
#include "physics/PhysicsManager.h"
#include "entity/Entity.h"
#include "entity/Renderable.h"
#include "entity/DynamicPhysicsObject.h"
#include "entity/StaticPhysicsObject.h"
#include "entity/Car.h"
#include "entity/AICar.h"
#include "entity/Hook.h"
#include "entity/Obstacle.h"
#include "entity/RectTrigger.h"

#include "Jukebox.h"
#include <ctime>
#include "util/ConfigParser.h"

using namespace std;

int main(int argc, const char* argv[])
{
    std::unique_ptr<Window> window(new Window(1024, 768));

	//Music
	Jukebox *bgm = new Jukebox("assets/sound/dmw.mp3");
	bgm->play();

    // Set up input
    std::unique_ptr<Input> input(new Input(0));

    StickListener stickListener;
    TriggerListener triggerListener;
	PhysicsManager* myPhysics = new PhysicsManager(&triggerListener, &stickListener);
	// OpponentAI* myAI = new OpponentAI();

    // TODO: convert these to unique_ptrs
    std::vector<Entity*> entities(0);
    std::vector<Car*> cars(0);

    Car *car = new Car("assets/models/car/testcar.obj", "assets/models/car/testcar_s1.png", nullptr, myPhysics, input.get(), entities);
	AICar *bot = new AICar("assets/models/car/testcar.obj", "assets/models/car/testcar_s1.png", nullptr, myPhysics, entities);

    window->getRenderer()->getCam()->registerController(input.get());
    window->getRenderer()->getCam()->registerCar(car);
 
    cars.push_back(car);
    entities.push_back(car);

    car->setPos(-200, 10, -350);

	cars.push_back(bot);
	entities.push_back(bot);

    car->setRot(0.0, 1.57 / 2.0, 0.0);

	bot->setPos(-25, 100, 72);
	bot->setRot(0.0, 1.57 / 2.0, 0.0);
    
    StaticPhysicsObject * myTrack = new StaticPhysicsObject("assets/models/track/tracksurface.obj", "assets/models/track/blue.png", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
    entities.push_back(myTrack);
    StaticPhysicsObject * myTrackWalls = new StaticPhysicsObject("assets/models/track/trackwalls.obj", "assets/models/track/green.png", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
    entities.push_back(myTrackWalls);
    StaticPhysicsObject * myHookables = new StaticPhysicsObject("assets/models/track/trackhooks.obj", "assets/textures/trans_red.png", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_HOOKABLE, COLLISION_FLAG_HOOKABLE_AGAINST);
    entities.push_back(myHookables);
    
    myTrack->SIL_X_SCALE = 1.1;
    myTrack->SIL_Y_SCALE = 1.1;
    myTrack->SIL_Z_SCALE = 1.1;
    myTrack->scaleModels();
    myTrack->setSil(false);

    myTrackWalls->SIL_X_SCALE = 1.01;
    myTrackWalls->SIL_Y_SCALE = 1.01;
    myTrackWalls->SIL_Z_SCALE = 1.01;
    myTrackWalls->scaleModels();
    myTrackWalls->setSil(false);

    
    myHookables->SIL_X_SCALE = 1.01;
    myHookables->SIL_Y_SCALE = 1.01;
    myHookables->SIL_Z_SCALE = 1.01;
    myHookables->scaleModels();
    myHookables->setSil(false);

    //Renderable* plane = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/logo_tile.png");
    /*Renderable* wall1 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/stc.png");
	Renderable* wall2 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/stc.png");
	Renderable* wall3 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/hearthstone.png");
	Renderable* wall4 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/hearthstone.png");
    */
    //plane->setPos(0, 0, 0);
   // plane->scale(50, 50, 50);
    /*
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
	wall3->setRot(1.57, -1.57, 0);
	wall3->scale(50, 50, 50);

	//sun
	wall4->setPos(-500, -20, 0);
	wall4->setRot(1.57, 1.57, 0);
	wall4->scale(50, 50, 50);
    */
    //entities.push_back(plane);
	/*entities.push_back(wall1);
	entities.push_back(wall2); //back
	entities.push_back(wall3);
	entities.push_back(wall4);*/

    // Create a finish-line trigger
   RectTrigger * finishLine = new RectTrigger(myPhysics, "assets/models/track/green.png", 6., 4., 6., 0, true);
    entities.push_back(finishLine);
    finishLine->setPos(-150, 10, -350);
    finishLine->SIL_X_SCALE = 1.02;
    finishLine->SIL_Y_SCALE = 1.02;
    finishLine->SIL_Z_SCALE = 1.02;
    finishLine->scaleModels();

    //myPhysics->createGroundPlane();
    //myPhysics->mScene->addActor(*createDrivablePlane(myPhysics->mMaterial, myPhysics->mPhysics));
	/*myPhysics->createWallPlane(0,5,-500,0,1);
	myPhysics->createWallPlane(0, 5, 500, 0, -1);
	myPhysics->createWallPlane(500, 5, 0, -1, 0);
	myPhysics->createWallPlane(-500, 5,  0, 1, 0);*/

    while (!window->shouldClose())
	{
        input->Update();
        bot->update();
		
        for (const auto& c : cars)
            c->stepForPhysics();
		myPhysics->stepPhysics();
   
        car->update();
       
	// mySound->updateSound();
		window->draw(entities);
	}

	delete myPhysics;
	return 0;
}