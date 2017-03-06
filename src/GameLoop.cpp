#define SDL_MAIN_HANDLED

#include <iostream>
#include "BuildFlags.h"

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

    car->setPos(-300, 10, -200);

	cars.push_back(bot);
	entities.push_back(bot);

    car->setRot(0.0, -0.5, 0.0);

	bot->setPos(-325, 10, -225);
	bot->setRot(0.0, -1.2, 0.0);
    
#ifdef FAST_TRACK
    StaticPhysicsObject * myTrack = new StaticPhysicsObject("assets/models/track/fast_track.obj", "assets/models/track/blue.png", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
#else
    StaticPhysicsObject * myTrack = new StaticPhysicsObject("assets/models/track/tracksurface.obj", "assets/models/track/blue.png", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
#endif
    entities.push_back(myTrack);
    StaticPhysicsObject * myTrackWalls = new StaticPhysicsObject("assets/models/track/trackwalls.obj", "assets/models/track/green.png", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
    entities.push_back(myTrackWalls);
    StaticPhysicsObject * myHookables = new StaticPhysicsObject("assets/models/track/trackhooks.obj", "assets/textures/trans_red.png", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_HOOKABLE, COLLISION_FLAG_HOOKABLE_AGAINST);
    entities.push_back(myHookables);
    
    myTrack->SIL_X_SCALE = 1.1;
    myTrack->SIL_Y_SCALE = 1.1;
    myTrack->SIL_Z_SCALE = 1.1;
    myTrack->SIL_JITTER = 1.2;
    myTrack->scaleModels();
    myTrack->setSil(true);

    myTrackWalls->SIL_X_SCALE = 1.01;
    myTrackWalls->SIL_Y_SCALE = 1.01;
    myTrackWalls->SIL_Z_SCALE = 1.01;
    myTrackWalls->SIL_JITTER = 1.0;
    myTrackWalls->scaleModels();
    myTrackWalls->setSil(true);

    
    myHookables->SIL_X_SCALE = 1.1;
    myHookables->SIL_Y_SCALE = 1.0;
    myHookables->SIL_Z_SCALE = 1.1;
    myHookables->SIL_JITTER = 1.0;
    myHookables->scaleModels();
    myHookables->setSil(false);

    //Renderable* plane = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/logo_tile.png");
    /*Renderable* wall1 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/stc.png");
	Renderable* wall2 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/stc.png");
	Renderable* wall3 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/hearthstone.png");
	Renderable* wall4 = new Renderable("assets/models/plane/plane.obj", "assets/models/plane/hearthstone.png");
    */
    //plane->setPos(0, 1200, 0);
    //plane->scale(50, 50, 50);
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
    RectTrigger * finishLine = new RectTrigger(myPhysics, "assets/textures/checkers.png", 10., 20., 100., 0, true);

    entities.push_back(finishLine);
    finishLine->setPos(-150, 0, -350);
    finishLine->scaleModels();
    finishLine->setSil(false);

    RectTrigger * checkpoint1 = new RectTrigger(myPhysics, "assets/models/track/green.png", 50., 20., 100., 1);

    entities.push_back(checkpoint1);
    checkpoint1->setPos(80.1522, 61.2789, 161.581);
    checkpoint1->SIL_X_SCALE = 1.02;
    checkpoint1->SIL_Y_SCALE = 1.02;
    checkpoint1->SIL_Z_SCALE = 1.02;
    checkpoint1->scaleModels();

    RectTrigger * checkpoint2 = new RectTrigger(myPhysics, "assets/models/track/green.png", 100., 20., 50., 2);

    entities.push_back(checkpoint2);
    checkpoint2->setPos(-950.594, 13.4629, -207.866);
    checkpoint2->SIL_X_SCALE = 1.02;
    checkpoint2->SIL_Y_SCALE = 1.02;
    checkpoint2->SIL_Z_SCALE = 1.02;
    checkpoint2->scaleModels();
    //myPhysics->createGroundPlane();
    //myPhysics->mScene->addActor(*createDrivablePlane(myPhysics->mMaterial, myPhysics->mPhysics));
	/*myPhysics->createWallPlane(0,5,-500,0,1);
	myPhysics->createWallPlane(0, 5, 500, 0, -1);
	myPhysics->createWallPlane(500, 5, 0, -1, 0);
	myPhysics->createWallPlane(-500, 5,  0, 1, 0);*/
    glfwSetTime(0);
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