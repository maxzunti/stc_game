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

#include <ctime>
#include "util/ConfigParser.h"

GameState::GameState()
{
}

GameState::GameState(Input * newInput, PhysicsManager * newPhysics)
{
    input = newInput;
    myPhysics = newPhysics;
}

GameState::GameState(Input * newInput, PhysicsManager * newPhysics, Jukebox * music)
{
    input = newInput;
    myPhysics = newPhysics;
    jb = music;
}

GameState::GameState(std::vector<Input*> newInputs, PhysicsManager * newPhysics, Jukebox * music )
{
    inputs = newInputs;
    myPhysics = newPhysics;
    jb = music;
}


GameState::~GameState()
{
}

void GameState::updateState(State state)
{
    currentState = state;
}

void GameState::initGame(int numberOfPlayers)
{
    

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

    StaticPhysicsObject * myHookables = new StaticPhysicsObject("assets/models/track/trackhooks.obj", "assets/textures/trans_red.png", glm::vec3(50.f, 50.f, 50.f), myPhysics, COLLISION_FLAG_HOOKABLE, COLLISION_FLAG_HOOKABLE_AGAINST, false);
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




    //  window->getRenderer()->getCam()->registerController(input.get());
    //  window->getRenderer()->getCam()->registerCar(car);

#ifdef SPLITSCREEN
    window->setSplitScreen(2, cars);

    // TODO: stop using the same controller for both cars
    window->getRenderer(1)->getCam()->registerController(input2.get());
#endif



    // Create a finish-line trigger
    RectTrigger * finishLine = new RectTrigger(myPhysics, "assets/textures/checkers.png", 10., 20., 100., 0, glm::vec3(0, 0, 0), true);

    entities.push_back(finishLine);
    finishLine->setPos(-150, 0, -350);
    finishLine->scaleModels();
    finishLine->setSil(false);

    RectTrigger * checkpoint0 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 1, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint0);
    checkpoint0->setPos(582.958, 2.29435, -153.876);
    checkpoint0->setRot(glm::quat(0.511569, -2.44891e-05, -0.859242, -0.000041213));
    checkpoint0->SIL_X_SCALE = 1.02;
    checkpoint0->SIL_Y_SCALE = 1.02;
    checkpoint0->SIL_Z_SCALE = 1.02;
    checkpoint0->scaleModels();


    RectTrigger * checkpoint1 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 2, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint1);
    checkpoint1->setPos(693.253, 2.2958, -46.3774);
    checkpoint1->setRot(glm::quat(0.186877, -0.000007764, -0.982383, -4.09262e-05));
    checkpoint1->SIL_X_SCALE = 1.02;
    checkpoint1->SIL_Y_SCALE = 1.02;
    checkpoint1->SIL_Z_SCALE = 1.02;
    checkpoint1->scaleModels();

    RectTrigger * checkpoint2 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 3, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint2);
    checkpoint2->setPos(705.173, 2.29011, 104.472);
    checkpoint2->setRot(glm::quat(-0.137021, 0.000004984, -0.990568, -0.000025414));
    checkpoint2->SIL_X_SCALE = 1.02;
    checkpoint2->SIL_Y_SCALE = 1.02;
    checkpoint2->SIL_Z_SCALE = 1.02;
    checkpoint2->scaleModels();

    RectTrigger * checkpoint3 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 4, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint3);
    checkpoint3->setPos(627.675, 2.30467, 242.062);
    checkpoint3->setRot(glm::quat(-0.421243, 0.000119609, -0.906948, -0.000261445));
    checkpoint3->SIL_X_SCALE = 1.02;
    checkpoint3->SIL_Y_SCALE = 1.02;
    checkpoint3->SIL_Z_SCALE = 1.02;
    checkpoint3->scaleModels();



    RectTrigger * checkpoint4 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 5, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint4);
    checkpoint4->setPos(479.333, 2.96775, 297.108);
    checkpoint4->setRot(glm::quat(-0.678676, -0.00527144, -0.734397, 0.00570051));
    checkpoint4->SIL_X_SCALE = 1.02;
    checkpoint4->SIL_Y_SCALE = 1.02;
    checkpoint4->SIL_Z_SCALE = 1.02;
    checkpoint4->scaleModels();

    RectTrigger * checkpoint5 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 6, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint5);
    checkpoint5->setPos(367.038, 6.9704, 292.648);
    checkpoint5->setRot(glm::quat(-0.773841, -0.0194649, -0.632846, 0.0172303));
    checkpoint5->SIL_X_SCALE = 1.02;
    checkpoint5->SIL_Y_SCALE = 1.02;
    checkpoint5->SIL_Z_SCALE = 1.02;
    checkpoint5->scaleModels();

    ///////////

    RectTrigger * checkpoint6 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 7, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint6);
    checkpoint6->setPos(66.205, 55.1288, 151.158);
    checkpoint6->setRot(glm::quat(-0.89978, -0.0834907, -0.426147, 0.042718));
    checkpoint6->SIL_X_SCALE = 1.02;
    checkpoint6->SIL_Y_SCALE = 1.02;
    checkpoint6->SIL_Z_SCALE = 1.02;
    checkpoint6->scaleModels();

    RectTrigger * checkpoint7 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 8, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint7);
    checkpoint7->setPos(-546.029, 84.1273, -358.687);
    checkpoint7->setRot(glm::quat(0.859496, -0.0671312, 0.50529, 0.0379721));
    checkpoint7->SIL_X_SCALE = 1.02;
    checkpoint7->SIL_Y_SCALE = 1.02;
    checkpoint7->SIL_Z_SCALE = 1.02;
    checkpoint7->scaleModels();

    RectTrigger * checkpoint8 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 9, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint8);
    checkpoint8->setPos(-705.506, 57.0474, -408.905);
    checkpoint8->setRot(glm::quat(0.6981, -0.0553387, 0.712099, 0.0500909));
    checkpoint8->SIL_X_SCALE = 1.02;
    checkpoint8->SIL_Y_SCALE = 1.02;
    checkpoint8->SIL_Z_SCALE = 1.02;
    checkpoint8->scaleModels();

    RectTrigger * checkpoint9 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 10, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint9);
    checkpoint9->setPos(-865.046, 26.1215, -367.016);
    checkpoint9->setRot(glm::quat(0.427347, -0.047236, 0.898499, 0.0885616));
    checkpoint9->SIL_X_SCALE = 1.02;
    checkpoint9->SIL_Y_SCALE = 1.02;
    checkpoint9->SIL_Z_SCALE = 1.02;
    checkpoint9->scaleModels();

    RectTrigger * checkpoint10 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 11, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint10);
    checkpoint10->setPos(-950.594, 13.4629, -207.866);
    checkpoint10->SIL_X_SCALE = 1.02;
    checkpoint10->SIL_Y_SCALE = 1.02;
    checkpoint10->SIL_Z_SCALE = 1.02;
    checkpoint10->scaleModels();

    RectTrigger * checkpoint11 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 12, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint11);
    checkpoint11->setPos(-883.837, 2.28911, 122.058);
    checkpoint11->setRot(glm::quat(0.272631, -0.00000391244, -0.962119, -0.0000143151));
    checkpoint11->SIL_X_SCALE = 1.02;
    checkpoint11->SIL_Y_SCALE = 1.02;
    checkpoint11->SIL_Z_SCALE = 1.02;
    checkpoint11->scaleModels();

    RectTrigger * checkpoint12 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 13, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint12);
    checkpoint12->setPos(-744.606, 2.29449, 252.435);
    checkpoint12->setRot(glm::quat(0.527757, -0.0000290464, -0.849395, -0.0000472041));
    checkpoint12->SIL_X_SCALE = 1.02;
    checkpoint12->SIL_Y_SCALE = 1.02;
    checkpoint12->SIL_Z_SCALE = 1.02;
    checkpoint12->scaleModels();

    RectTrigger * checkpoint13 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 14, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint13);
    checkpoint13->setPos(-573.404, 2.29473, 290.003);
    checkpoint13->setRot(glm::quat(0.722942, -0.0000329909, -0.690909, -0.000031511));
    checkpoint13->SIL_X_SCALE = 1.02;
    checkpoint13->SIL_Y_SCALE = 1.02;
    checkpoint13->SIL_Z_SCALE = 1.02;
    checkpoint13->scaleModels();

    RectTrigger * checkpoint14 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 15, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint14);
    checkpoint14->setPos(-420.546, 2.29476, 239.476);
    checkpoint14->setRot(glm::quat(0.90891, -0.000028612, -0.416991, -0.0000129683));
    checkpoint14->SIL_X_SCALE = 1.02;
    checkpoint14->SIL_Y_SCALE = 1.02;
    checkpoint14->SIL_Z_SCALE = 1.02;
    checkpoint14->scaleModels();

    RectTrigger * checkpoint15 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 16, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint15);
    checkpoint15->setPos(-380.952, 2.29209, 176.553);
    checkpoint15->setRot(glm::quat(0.986375, -0.000120485, -0.164515, -0.0000177607));
    checkpoint15->SIL_X_SCALE = 1.02;
    checkpoint15->SIL_Y_SCALE = 1.02;
    checkpoint15->SIL_Z_SCALE = 1.02;
    checkpoint15->scaleModels();

    RectTrigger * checkpoint16 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 17, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint16);
    checkpoint16->setPos(-354.737, 1.3471, -186.251);
    checkpoint16->setRot(glm::quat(0.985539, -0.00253063, -0.169429, -0.000370684));
    checkpoint16->SIL_X_SCALE = 1.02;
    checkpoint16->SIL_Y_SCALE = 1.02;
    checkpoint16->SIL_Z_SCALE = 1.02;
    checkpoint16->scaleModels();

    RectTrigger * checkpoint17 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 18, glm::vec3(0, 0, 0), false);
    entities.push_back(checkpoint17);
    checkpoint17->setPos(-310.232, 1.00723, -270.753);
    checkpoint17->setRot(glm::quat(0.933793, -0.000355677, -0.357813, -0.000113032));
    checkpoint17->SIL_X_SCALE = 1.02;
    checkpoint17->SIL_Y_SCALE = 1.02;
    checkpoint17->SIL_Z_SCALE = 1.02;
    checkpoint17->scaleModels();

    trackNodes.push_back(finishLine);
    trackNodes.push_back(checkpoint0);
    trackNodes.push_back(checkpoint1);
    trackNodes.push_back(checkpoint2);
    trackNodes.push_back(checkpoint3);
    trackNodes.push_back(checkpoint4);
    trackNodes.push_back(checkpoint5);
    trackNodes.push_back(checkpoint6);
    trackNodes.push_back(checkpoint7);
    trackNodes.push_back(checkpoint8);
    trackNodes.push_back(checkpoint9);
    trackNodes.push_back(checkpoint10);
    trackNodes.push_back(checkpoint11);
    trackNodes.push_back(checkpoint12);
    trackNodes.push_back(checkpoint13);
    trackNodes.push_back(checkpoint14);
    trackNodes.push_back(checkpoint15);
    trackNodes.push_back(checkpoint16);
    trackNodes.push_back(checkpoint17);


    for (int i = 0; i < numberOfPlayers; i++) {
        Car *car = new Car(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, inputs[i], entities, jb, myTrack, trackNodes);
        cars.push_back(car);
        entities.push_back(car);
    }

    for (int i = 0; i < (4 - numberOfPlayers); i++) {
        AICar *bot = new AICar(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, entities, myTrack, trackNodes);
        cars.push_back(bot);
        entities.push_back(bot);
    }
    //  Car *car2 = new Car(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, input2, entities, jb, myTrack, trackNodes);
    // cars.push_back(car2);
    // entities.push_back(car2);
/*
    AICar *bot = new AICar(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, entities, myTrack, trackNodes);
    cars.push_back(bot);
    entities.push_back(bot);

    AICar *bot2 = new AICar(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(PURPLE), nullptr, myPhysics, entities, myTrack, trackNodes);
    cars.push_back(bot2);
    entities.push_back(bot2);
//*/
//
//    car->setPos(-300, 10, -200);
//    car->setRot(0.0, -0.5, 0.0);
//
//    //  car2->setPos(-300, 10, -250);
//    //  car2->setRot(0.0, -0.5, 0.0);
//
//    bot->setPos(-325, 10, -225);
//    bot->setRot(0.0, -1.2, 0.0);
//
//    bot2->setPos(-350, 10, -250);
//    bot2->setRot(0.0, -1.2, 0.0);

    for (int i = 0; i < cars.size(); i ++) {
        cars[i]->setPos(-300 - (25*i), 10, -200 - (25 * i));
        cars[i]->setRot(0.0, -0.5, 0.0);
    }

    RectTrigger * hookpoint = new RectTrigger(myPhysics, "assets/textures/trans_red.png", 90., 20., 10., RectTrigger::HOOKZONE, glm::vec3(-483.459, 110.2245, -265.981), false);
    entities.push_back(hookpoint);
    hookpoint->setPos(-239.009, 102.55, -110.475);
    hookpoint->setRot(glm::quat(-0.906763, -0.0235591, -0.420852, 0.0104264));
    hookpoint->SIL_X_SCALE = 1.02;
    hookpoint->SIL_Y_SCALE = 1.02;
    hookpoint->SIL_Z_SCALE = 1.02;
    hookpoint->scaleModels();

    RectTrigger * fallpoint = new RectTrigger(myPhysics, "assets/textures/trans_red.png", 90., 5., 120., RectTrigger::FALLZONE, glm::vec3(0,0,0), false);
    entities.push_back(fallpoint);
    fallpoint->setPos(-333.785, 80.24237, -201.133);
    fallpoint->setRot(glm::quat(-0.925566, 0.0023519, 0.378579, -0.0000170884));
    fallpoint->SIL_X_SCALE = 1.02;
    fallpoint->SIL_Y_SCALE = 1.02;
    fallpoint->SIL_Z_SCALE = 1.02;
    fallpoint->scaleModels();


    jb->loadMusic("assets/sound/dmw.mp3");
    jb->play();
        
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
}

void GameState::endGame() {
    int i = 0;
    
    for (Entity* e : entities)
    {
        std::cout << entities.size() << std::endl;
        delete e;
    }
    entities.clear();
    cars.clear();
    trackNodes.clear();
    
}
