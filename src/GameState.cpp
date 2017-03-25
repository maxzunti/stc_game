#include "GameState.h"

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
#include "GameState.h"

#include "Jukebox.h"
#include <ctime>
#include "util/ConfigParser.h"
#include "GameLoop.h"

GameState::GameState()
{
}

GameState::GameState(Input * newInput, PhysicsManager * newPhysics)
{
    input = newInput;
    myPhysics = newPhysics;
}

GameState::~GameState()
{
}

void GameState::updateState(State state)
{
    currentState = state;
}

void GameState::initGame()
{
    //Music
    Jukebox *jb = new Jukebox();
    jb->setup();
    //jb->play();

    ///////////////////////////////////////////////////////////////////////////////////////
    /// THE FOLLOWING CAN BE DONE IN THE GAME STATE                                     ///
    ///////////////////////////////////////////////////////////////////////////////////////

   // StickListener stickListener;
   // TriggerListener triggerListener;
    //OpponentAI* myAI = new OpponentAI();

    /*
    // TODO: convert these to unique_ptrs
    std::vector<Entity*> entities(0);
    std::vector<Car*> cars(0);
    */

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

    Car *car = new Car(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, input, entities, jb, myTrack);
    cars.push_back(car);
    entities.push_back(car);
    
   /* Car *car2 = new Car(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, input, entities, jb, myTrack);
    cars.push_back(car2);
    entities.push_back(car2);
    */
    AICar *bot = new AICar(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, entities, myTrack);
    cars.push_back(bot);
    entities.push_back(bot);

    car->setPos(-300, 10, -200);
    car->setRot(0.0, -0.5, 0.0);

   // car2->setPos(-300, 10, -250);
    //car2->setRot(0.0, -0.5, 0.0);

    bot->setPos(-325, 10, -225);
    bot->setRot(0.0, -1.2, 0.0);


#ifdef SPLITSCREEN
    window->setSplitScreen(2, cars);

    // TODO: stop using the same controller for both cars
    window->getRenderer(1)->getCam()->registerController(input.get());
#endif

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
    //glfwSetTime(0);


    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
}

void GameState::endGame() {
    entities.clear();
    cars.clear();
}
