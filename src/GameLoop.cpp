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
#include "entity/Track.h"
#include "entity/Walls.h"
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
    std::unique_ptr<Window> window(new Window(1280, 720));

	//Music
	Jukebox *jb = new Jukebox();
    jb->setup();
    jb->play();

    // Set up input
    std::unique_ptr<Input> input(new Input(0));

    StickListener stickListener;
    TriggerListener triggerListener;
	PhysicsManager* myPhysics = new PhysicsManager(&triggerListener, &stickListener);
	// OpponentAI* myAI = new OpponentAI();

    // TODO: convert these to unique_ptrs
    std::vector<Entity*> entities(0);
    std::vector<Car*> cars(0);


#ifdef FAST_TRACK
    Track * myTrack = new Track("assets/models/track/fast_track.obj", "assets/textures/alum.png", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
    for (auto m : myTrack->getModels()) {
    //    m->tile_UV_Y(30);
    //    m->tile_UV_X(2);
    }
#else
    Track * myTrack = new Track("assets/models/track/tracksurface.obj", "assets/textures/alum.png", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
#endif
    entities.push_back(myTrack);
    Walls * myTrackWalls = new Walls("assets/models/track/trackwalls.obj", "assets/textures/tris.jpg", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
    myTrackWalls->height = -10; // moving walls UP at the moment - depends on model
    entities.push_back(myTrackWalls);
    for (auto m : myTrackWalls->getModels()) {
        m->tile_UV_XY(2, 2);
    }
    for (auto m : myTrack->getModels()) {
            m->tile_UV_XY(3, 60);
    }

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
    myHookables->setSil(true);

    // Create a finish-line trigger
    RectTrigger * finishLine = new RectTrigger(myPhysics, "assets/textures/checkers.png", 10., 20., 100., 0, true);

    entities.push_back(finishLine);
    finishLine->setPos(-150, 0, -350);
    finishLine->scaleModels();
    finishLine->setSil(false);

    RectTrigger * checkpoint0 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 1, true);
    entities.push_back(checkpoint0);
    checkpoint0->setPos(582.958, 2.29435, -153.876);
    checkpoint0->setRot(glm::quat(0.511569, -2.44891e-05, -0.859242, -0.000041213));
    checkpoint0->SIL_X_SCALE = 1.02;
    checkpoint0->SIL_Y_SCALE = 1.02;
    checkpoint0->SIL_Z_SCALE = 1.02;
    checkpoint0->scaleModels();


    RectTrigger * checkpoint1 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 2, true);
    entities.push_back(checkpoint1);
    checkpoint1->setPos(693.253, 2.2958, -46.3774);
    checkpoint1->setRot(glm::quat(0.186877, -0.000007764, -0.982383, -4.09262e-05));
    checkpoint1->SIL_X_SCALE = 1.02;
    checkpoint1->SIL_Y_SCALE = 1.02;
    checkpoint1->SIL_Z_SCALE = 1.02;
    checkpoint1->scaleModels();

    RectTrigger * checkpoint2 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 3, true);
    entities.push_back(checkpoint2);
    checkpoint2->setPos(705.173, 2.29011, 104.472);
    checkpoint2->setRot(glm::quat(-0.137021, 0.000004984, -0.990568, -0.000025414));
    checkpoint2->SIL_X_SCALE = 1.02;
    checkpoint2->SIL_Y_SCALE = 1.02;
    checkpoint2->SIL_Z_SCALE = 1.02;
    checkpoint2->scaleModels();

    RectTrigger * checkpoint3 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 4, true);
    entities.push_back(checkpoint3);
    checkpoint3->setPos(665.245, 2.31042, 199.581);
    checkpoint3->setRot(glm::quat(-0.352183, -0.00000424364, -0.935931, 0.0000363322));
    checkpoint3->SIL_X_SCALE = 1.02;
    checkpoint3->SIL_Y_SCALE = 1.02;
    checkpoint3->SIL_Z_SCALE = 1.02;
    checkpoint3->scaleModels();

    ///////////
    

    RectTrigger * checkpoint4 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 5, true);
    entities.push_back(checkpoint4);
    checkpoint4->setPos(66.205, 55.1288, 151.158);
    checkpoint4->setRot(glm::quat(-0.89978, -0.0834907, -0.426147, 0.042718));
    checkpoint4->SIL_X_SCALE = 1.02;
    checkpoint4->SIL_Y_SCALE = 1.02;
    checkpoint4->SIL_Z_SCALE = 1.02;
    checkpoint4->scaleModels();

    RectTrigger * checkpoint5 = new RectTrigger(myPhysics, "assets/models/track/green.png", 100., 20., 50., 6, true);

    entities.push_back(checkpoint5);
    checkpoint5->setPos(-950.594, 13.4629, -207.866);
    checkpoint5->SIL_X_SCALE = 1.02;
    checkpoint5->SIL_Y_SCALE = 1.02;
    checkpoint5->SIL_Z_SCALE = 1.02;
    checkpoint5->scaleModels();

    std::vector<RectTrigger*> trackNodes;
    trackNodes.push_back(finishLine);
    trackNodes.push_back(checkpoint0);
    trackNodes.push_back(checkpoint1);
    trackNodes.push_back(checkpoint2);
    trackNodes.push_back(checkpoint3);
    trackNodes.push_back(checkpoint4);
    trackNodes.push_back(checkpoint5);
    Car *car = new Car(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, input.get(), entities, jb, myTrack);
    cars.push_back(car);
    entities.push_back(car);

    Car *car2 = new Car(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, input.get(), entities, jb, myTrack);
    cars.push_back(car2);
    entities.push_back(car2);

    AICar *bot = new AICar(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, entities, myTrack, trackNodes);
    cars.push_back(bot);
    entities.push_back(bot);

    car->setPos(-300, 10, -200);
    car->setRot(0.0, -0.5, 0.0);

    car2->setPos(-300, 10, -250);
    car2->setRot(0.0, -0.5, 0.0);

    bot->setPos(-325, 10, -225);
    bot->setRot(0.0, -1.2, 0.0);


    window->getRenderer()->getCam()->registerController(input.get());
    window->getRenderer()->getCam()->registerCar(car);

#ifdef SPLITSCREEN
    window->setSplitScreen(2, cars);

    // TODO: stop using the same controller for both cars
    window->getRenderer(1)->getCam()->registerController(input.get());
#endif



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
		window->draw(entities, cars);
	}

	delete myPhysics;
	return 0;
}