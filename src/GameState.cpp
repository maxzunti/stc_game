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
    countDownLength = countDownMax;
}

GameState::GameState(Input * newInput, PhysicsManager * newPhysics, Jukebox * music)
{
    input = newInput;
    myPhysics = newPhysics;
    jb = music;
    countDownLength = countDownMax;
}

GameState::GameState(std::vector<Input*> newInputs, PhysicsManager * newPhysics, Jukebox * music )
{
    inputs = newInputs;
    myPhysics = newPhysics;
    jb = music;
    countDownLength = countDownMax;
}


GameState::~GameState()
{
}

void GameState::updateState(State state)
{
    currentState = state;
}

bool GameState::isRaceComplete()
{
    int complete = 0;
    for (auto c : cars) {
        if (c->doneRace) {
            complete++;
        }
    }
    if (complete == 4) {
        return true;
    }
    else {
        return false;
    }
}
void GameState::initGame(int numberOfPlayers, int selectedTrack)
{
    
    switch (selectedTrack) {
    case 1: {
        trackScale = 50.f;
        myTrack = new Track("assets/models/track/tracksurface.obj", "assets/textures/alum.png", glm::vec3(trackScale, trackScale, trackScale), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
        entities.push_back(myTrack);

        Walls * myTrackWalls = new Walls("assets/models/track/trackwalls.obj", "assets/textures/tris.jpg", glm::vec3(trackScale, trackScale, trackScale), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
        myTrackWalls->height = -10; // moving walls UP at the moment - depends on model
        entities.push_back(myTrackWalls);
        for (auto m : myTrackWalls->getModels()) {
            m->tile_UV_XY(2, 2);
        }
        for (auto m : myTrack->getModels()) {
            m->tile_UV_XY(3, 60); // map 1 is 3,60
        }

        StaticPhysicsObject * myHookables = new StaticPhysicsObject("assets/models/track/trackhooks2.obj", "assets/textures/trans_red.png", glm::vec3(trackScale, trackScale, trackScale), myPhysics, COLLISION_FLAG_HOOKABLE, COLLISION_FLAG_HOOKABLE_AGAINST, false);
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

        Renderable * finishflag = new Renderable("assets/models/Crate/Crate1.obj", "assets/textures/checkers.png");
        entities.push_back(finishflag);
        finishflag->setPos(-160.176, 33.68377, -334.572);
        finishflag->scale(10., 10., 80.);
        finishflag->scaleModels();
        finishflag->setSil(false);

        // Create a finish-line trigger
        RectTrigger * finishLine = new RectTrigger(myPhysics, "assets/textures/checkers.png", 90., 20., 10., 0, glm::vec3(0, 0, 0), false);

        entities.push_back(finishLine);
        finishLine->setPos(-160.176, 0, -334.572);
        finishLine->setRot(glm::quat(0.712873, 0.00192498, -0.701288, 0.00172225));
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
            Car *car = new Car(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(CarColor(i)), nullptr, myPhysics, inputs[i], entities, jb, myTrack, trackNodes);
            cars.push_back(car);
            entities.push_back(car);
            car->engineSoundChannel = 100 + 5 * i;
            car->idleSoundChannel = 101 + 5 * i;
            car->hornSoundChannel = 102 + 5 * i;
        }

        for (int i = 0; i < (4 - numberOfPlayers); i++) {
            AICar *bot = new AICar(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(CarColor(i + numberOfPlayers)), nullptr, myPhysics, entities, myTrack, trackNodes);
            cars.push_back(bot);
            entities.push_back(bot);
        }

        for (int i = 0; i < cars.size(); i++) {
            cars[i]->setPos(-300 - (25 * i), 10, -200 - (25 * i)); //these are map 1
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

        RectTrigger * fallpoint = new RectTrigger(myPhysics, "assets/textures/trans_red.png", 90., 5., 120., RectTrigger::FALLZONE, glm::vec3(0, 0, 0), false);
        entities.push_back(fallpoint);
        fallpoint->setPos(-333.785, 80.24237, -201.133);
        fallpoint->setRot(glm::quat(-0.925566, 0.0023519, 0.378579, -0.0000170884));
        fallpoint->SIL_X_SCALE = 1.02;
        fallpoint->SIL_Y_SCALE = 1.02;
        fallpoint->SIL_Z_SCALE = 1.02;
        fallpoint->scaleModels();


        jb->loadMusic("assets/sound/arene.mp3");

        break;
    }
    case 2: {
        trackScale = 250.0f;
        myTrack = new Track("assets/models/track/track2surface2.obj", "assets/textures/alum.png", glm::vec3(trackScale, trackScale, trackScale), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
        entities.push_back(myTrack);

        myTrack->setPos(myTrack->getPos().x, myTrack->getPos().y + 600, myTrack->getPos().z);
        for (auto m : myTrack->getModels()) {
            m->tile_UV_XY(5000, 5000); // davids is 4000, 4000
        }

        StaticPhysicsObject * myHookables = new StaticPhysicsObject("assets/models/track/track2hooks.obj", "assets/textures/hex.png", glm::vec3(trackScale, trackScale, trackScale), myPhysics, COLLISION_FLAG_HOOKABLE, COLLISION_FLAG_HOOKABLE_AGAINST, false);
        entities.push_back(myHookables);
        myHookables->setPos(myHookables->getPos().x, myHookables->getPos().y + 600, myHookables->getPos().z);

        Walls * myTrackWalls = new Walls("assets/models/track/track2walls3.obj", "assets/textures/tris.jpg", glm::vec3(trackScale, trackScale, trackScale), myPhysics, COLLISION_FLAG_GROUND, COLLISION_FLAG_GROUND_AGAINST);
        //myTrackWalls->height = -10; // moving walls UP at the moment - depends on model
        entities.push_back(myTrackWalls);
        for (auto m : myTrackWalls->getModels()) {
            m->tile_UV_XY(10, 10);
        }
        myTrackWalls->setPos(myTrackWalls->getPos().x, myTrackWalls->getPos().y + 600, myTrackWalls->getPos().z);

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

        Renderable * finishflag = new Renderable("assets/models/Crate/Crate1.obj", "assets/textures/checkers.png");
        entities.push_back(finishflag);
        finishflag->setPos(45.1907, 443.264, -1160);
        // finishflag->setRot(glm::quat(-0.691184, -0.000337045, 0.722679, 0.000411464));
        finishflag->scale(10., 10., 80.);
        finishflag->scaleModels();
        finishflag->setSil(false);
        // Create a finish-line trigger
        RectTrigger * finishLine = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 0, glm::vec3(0, 0, 0), false);

        entities.push_back(finishLine);
        finishLine->setPos(45.1907, 413.264, -1160);
        finishLine->setRot(glm::quat(-0.691184, -0.000337045, 0.722679, 0.000411464));
        finishLine->scaleModels();
        finishLine->setSil(false);


        RectTrigger * checkpoint0 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 1, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint0);
        checkpoint0->setPos(851.942, 403.43, -1061.25);
        checkpoint0->setRot(glm::quat(-0.635535, -0.000828615, 0.772072, 0.000744174));
        checkpoint0->SIL_X_SCALE = 1.02;
        checkpoint0->SIL_Y_SCALE = 1.02;
        checkpoint0->SIL_Z_SCALE = 1.02;
        checkpoint0->scaleModels();


        RectTrigger * checkpoint00 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 2, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint00);
        checkpoint00->setPos(1029.22, 403.418, -983.928);
        checkpoint00->setRot(glm::quat(-0.413679, -0.000952022, 0.910422, 0.000700592));
        checkpoint00->SIL_X_SCALE = 1.02;
        checkpoint00->SIL_Y_SCALE = 1.02;
        checkpoint00->SIL_Z_SCALE = 1.02;
        checkpoint00->scaleModels();


        RectTrigger * checkpoint1 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 3, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint1);
        checkpoint1->setPos(1161.5, 403.419, -836.285);
        checkpoint1->setRot(glm::quat(-0.252179, -0.00117974, 0.96768, 0.000331034));
        checkpoint1->SIL_X_SCALE = 1.02;
        checkpoint1->SIL_Y_SCALE = 1.02;
        checkpoint1->SIL_Z_SCALE = 1.02;
        checkpoint1->scaleModels();


        RectTrigger * checkpoint2 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 4, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint2);
        checkpoint2->setPos(1198.95, 403.426, -633.717);
        checkpoint2->setRot(glm::quat(0.0254999, -0.0012848, 0.999674, 1.99332e-05));
        checkpoint2->SIL_X_SCALE = 1.02;
        checkpoint2->SIL_Y_SCALE = 1.02;
        checkpoint2->SIL_Z_SCALE = 1.02;
        checkpoint2->scaleModels();


        RectTrigger * checkpoint3 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 5, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint3);
        checkpoint3->setPos(935.397, 403.496, 170.639);
        checkpoint3->setRot(glm::quat(0.0841971, -0.00150501, 0.996448, -0.000146874));
        checkpoint3->SIL_X_SCALE = 1.02;
        checkpoint3->SIL_Y_SCALE = 1.02;
        checkpoint3->SIL_Z_SCALE = 1.02;
        checkpoint3->scaleModels();

        RectTrigger * checkpoint03 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 6, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint03);
        checkpoint03->setPos(937.757, 403.534, 285.821);
        checkpoint03->setRot(glm::quat(-0.098353, -0.0015983, 0.99515, 0.000327442));
        checkpoint03->SIL_X_SCALE = 1.02;
        checkpoint03->SIL_Y_SCALE = 1.02;
        checkpoint03->SIL_Z_SCALE = 1.02;
        checkpoint03->scaleModels();


        RectTrigger * checkpoint4 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 7, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint4);
        checkpoint4->setPos(1049.36, 403.588, 436.127);
        checkpoint4->setRot(glm::quat(-0.378116, -0.00166287, 0.925757, 0.000742357));
        checkpoint4->SIL_X_SCALE = 1.02;
        checkpoint4->SIL_Y_SCALE = 1.02;
        checkpoint4->SIL_Z_SCALE = 1.02;
        checkpoint4->scaleModels();

        RectTrigger * checkpoint04 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 8, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint04);
        checkpoint04->setPos(1178.83, 403.603, 502.897);
        checkpoint04->setRot(glm::quat(-0.660583, -0.00153692, 0.750751, 0.0012668));
        checkpoint04->SIL_X_SCALE = 1.02;
        checkpoint04->SIL_Y_SCALE = 1.02;
        checkpoint04->SIL_Z_SCALE = 1.02;
        checkpoint04->scaleModels();

        RectTrigger * checkpoint5 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 9, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint5);
        checkpoint5->setPos(1346.73, 404.291, 497.862);
        checkpoint5->setRot(glm::quat(-0.812116, -0.00696297, 0.583438, -0.0043467));
        checkpoint5->SIL_X_SCALE = 1.02;
        checkpoint5->SIL_Y_SCALE = 1.02;
        checkpoint5->SIL_Z_SCALE = 1.02;
        checkpoint5->scaleModels();


        RectTrigger * checkpoint6 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 10, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint6);
        checkpoint6->setPos(1917.29, 470.585, -544.66);
        checkpoint6->setRot(glm::quat(-0.96893, -0.0289118, 0.244921, -0.0187887));
        checkpoint6->SIL_X_SCALE = 1.02;
        checkpoint6->SIL_Y_SCALE = 1.02;
        checkpoint6->SIL_Z_SCALE = 1.02;
        checkpoint6->scaleModels();

        RectTrigger * checkpoint7 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 11, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint7);
        checkpoint7->setPos(2246.95, 497.827, -874.965);
        checkpoint7->setRot(glm::quat(-0.864169, -0.022436, 0.502652, -0.007069));
        checkpoint7->SIL_X_SCALE = 1.02;
        checkpoint7->SIL_Y_SCALE = 1.02;
        checkpoint7->SIL_Z_SCALE = 1.02;
        checkpoint7->scaleModels();

        RectTrigger * checkpoint8 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 12, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint8);
        checkpoint8->setPos(2529.62, 509.353, -948.43);
        checkpoint8->setRot(glm::quat(-0.665854, -0.0227552, 0.745735, 0.000333642));
        checkpoint8->SIL_X_SCALE = 1.02;
        checkpoint8->SIL_Y_SCALE = 1.02;
        checkpoint8->SIL_Z_SCALE = 1.02;
        checkpoint8->scaleModels();


        RectTrigger * checkpoint9 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 13, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint9);
        checkpoint9->setPos(2790.64, 517.64, -863.81);
        checkpoint9->setRot(glm::quat(-0.459552, -0.0276645, 0.88772, -0.000367128));
        checkpoint9->SIL_X_SCALE = 1.02;
        checkpoint9->SIL_Y_SCALE = 1.02;
        checkpoint9->SIL_Z_SCALE = 1.02;
        checkpoint9->scaleModels();


        RectTrigger * checkpoint10 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 14, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint10);
        checkpoint10->setPos(2956.28, 524.273, -661.214);
        checkpoint10->setRot(glm::quat(0.224335, 0.0330071, -0.97394, 0.00499364));
        checkpoint10->SIL_X_SCALE = 1.02;
        checkpoint10->SIL_Y_SCALE = 1.02;
        checkpoint10->SIL_Z_SCALE = 1.02;
        checkpoint10->scaleModels();

        RectTrigger * checkpoint11 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 40., 10., 15, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint11);
        checkpoint11->setPos(3317.82, 559.709, 434.708);
        checkpoint11->setRot(glm::quat(0.0487342, 0.0374407, -0.998024, 0.0131145));
        checkpoint11->SIL_X_SCALE = 1.02;
        checkpoint11->SIL_Y_SCALE = 1.02;
        checkpoint11->SIL_Z_SCALE = 1.02;
        checkpoint11->scaleModels();

        RectTrigger * checkpoint12 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 16, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint12);
        checkpoint12->setPos(3295.49, 572.913, 770.407);
        checkpoint12->setRot(glm::quat(0.170264, -0.0415258, 0.983953, -0.0334893));
        checkpoint12->SIL_X_SCALE = 1.02;
        checkpoint12->SIL_Y_SCALE = 1.02;
        checkpoint12->SIL_Z_SCALE = 1.02;
        checkpoint12->scaleModels();


        RectTrigger * checkpoint13 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 17, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint13);
        checkpoint13->setPos(3194.86, 583.315, 923.358);
        checkpoint13->setRot(glm::quat(0.390337, -0.0432375, 0.91817, -0.0522691));
        checkpoint13->SIL_X_SCALE = 1.02;
        checkpoint13->SIL_Y_SCALE = 1.02;
        checkpoint13->SIL_Z_SCALE = 1.02;
        checkpoint13->scaleModels();


        RectTrigger * checkpoint14 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 18, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint14);
        checkpoint14->setPos(2614.93, 593.161, 1130.75);
        checkpoint14->setRot(glm::quat(-0.707193, 0.0508094, -0.703666, 0.0463685));
        checkpoint14->SIL_X_SCALE = 1.02;
        checkpoint14->SIL_Y_SCALE = 1.02;
        checkpoint14->SIL_Z_SCALE = 1.02;
        checkpoint14->scaleModels();


        RectTrigger * checkpoint15 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 19, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint15);
        checkpoint15->setPos(2207.44, 594.781, 1057.85);
        checkpoint15->setRot(glm::quat(-0.804016, 0.0341375, -0.590355, 0.0622367));
        checkpoint15->SIL_X_SCALE = 1.02;
        checkpoint15->SIL_Y_SCALE = 1.02;
        checkpoint15->SIL_Z_SCALE = 1.02;
        checkpoint15->scaleModels();


        RectTrigger * checkpoint16 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 20, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint16);
        checkpoint16->setPos(1794.46, 597.552, 796.104);
        checkpoint16->setRot(glm::quat(-0.909304, 0.0287123, -0.409878, 0.0658895));
        checkpoint16->SIL_X_SCALE = 1.02;
        checkpoint16->SIL_Y_SCALE = 1.02;
        checkpoint16->SIL_Z_SCALE = 1.02;
        checkpoint16->scaleModels();

        RectTrigger * checkpoint17 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 21, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint17);
        checkpoint17->setPos(1677.58, 597.6, 655.458);
        checkpoint17->setRot(glm::quat(-0.97573, 0.0189267, -0.206816, 0.069431));
        checkpoint17->SIL_X_SCALE = 1.02;
        checkpoint17->SIL_Y_SCALE = 1.02;
        checkpoint17->SIL_Z_SCALE = 1.02;
        checkpoint17->scaleModels();

        RectTrigger * checkpoint18 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 22, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint18);
        checkpoint18->setPos(1503.97, 595.131, 218.709);
        checkpoint18->setRot(glm::quat(-0.958984, 0.0128252, -0.274104, 0.071073));
        checkpoint18->SIL_X_SCALE = 1.02;
        checkpoint18->SIL_Y_SCALE = 1.02;
        checkpoint18->SIL_Z_SCALE = 1.02;
        checkpoint18->scaleModels();


        RectTrigger * checkpoint19 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 23, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint19);
        checkpoint19->setPos(1305.87, 600.762, -15.7002);
        checkpoint19->setRot(glm::quat(-0.824581, 0.0208122, -0.561831, 0.0630867));
        checkpoint19->SIL_X_SCALE = 1.02;
        checkpoint19->SIL_Y_SCALE = 1.02;
        checkpoint19->SIL_Z_SCALE = 1.02;
        checkpoint19->scaleModels();

        RectTrigger * checkpoint20 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 24, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint20);
        checkpoint20->setPos(1087.53, 613.554, 15.0379);
        checkpoint20->setRot(glm::quat(-0.514193, 0.0200037, -0.856109, 0.0477675));
        checkpoint20->SIL_X_SCALE = 1.02;
        checkpoint20->SIL_Y_SCALE = 1.02;
        checkpoint20->SIL_Z_SCALE = 1.02;
        checkpoint20->scaleModels();

        RectTrigger * checkpoint21 = new RectTrigger(myPhysics, "assets/models/track/green.png", 110., 110., 10., 25, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint21);
        checkpoint21->setPos(-329.379, 304.718, 939.671);
        checkpoint21->setRot(glm::quat(-0.510013, 0.0561306, -0.856977, -0.0482336));
        checkpoint21->SIL_X_SCALE = 1.02;
        checkpoint21->SIL_Y_SCALE = 1.02;
        checkpoint21->SIL_Z_SCALE = 1.02;
        checkpoint21->scaleModels();


        RectTrigger * checkpoint22 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 26, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint22);
        checkpoint22->setPos(-1245.36, 240.488, 1404.74);
        checkpoint22->setRot(glm::quat(0.563764, -0.0208359, 0.825534, -0.0151465));
        checkpoint22->SIL_X_SCALE = 1.02;
        checkpoint22->SIL_Y_SCALE = 1.02;
        checkpoint22->SIL_Z_SCALE = 1.02;
        checkpoint22->scaleModels();


        RectTrigger * checkpoint23 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 27, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint23);
        checkpoint23->setPos(-1504.38, 233.667, 1435.76);
        checkpoint23->setRot(glm::quat(0.740958, -0.0316373, 0.670804, 0.00177765));
        checkpoint23->SIL_X_SCALE = 1.02;
        checkpoint23->SIL_Y_SCALE = 1.02;
        checkpoint23->SIL_Z_SCALE = 1.02;
        checkpoint23->scaleModels();

        RectTrigger * checkpoint24 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 28, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint24);
        checkpoint24->setPos(-1678.61, 221.743, 1385.18);
        checkpoint24->setRot(glm::quat(0.89246, -0.0358419, 0.449399, 0.0164721));
        checkpoint24->SIL_X_SCALE = 1.02;
        checkpoint24->SIL_Y_SCALE = 1.02;
        checkpoint24->SIL_Z_SCALE = 1.02;
        checkpoint24->scaleModels();

        RectTrigger * checkpoint25 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 29, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint25);
        checkpoint25->setPos(-1927.61, 192.362, 1111.78);
        checkpoint25->setRot(glm::quat(0.977952, -0.027405, 0.20556, 0.0245538));
        checkpoint25->SIL_X_SCALE = 1.02;
        checkpoint25->SIL_Y_SCALE = 1.02;
        checkpoint25->SIL_Z_SCALE = 1.02;
        checkpoint25->scaleModels();


        RectTrigger * checkpoint26 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 30, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint26);
        checkpoint26->setPos(-2065.04, 182.702, 540.081);
        checkpoint26->setRot(glm::quat(-0.997539, -0.029193, 0.0604112, -0.0203497));
        checkpoint26->SIL_X_SCALE = 1.02;
        checkpoint26->SIL_Y_SCALE = 1.02;
        checkpoint26->SIL_Z_SCALE = 1.02;
        checkpoint26->scaleModels();

        RectTrigger * checkpoint27 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 31, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint27);
        checkpoint27->setPos(-1947.71, 300.154, -95.3527);
        checkpoint27->setRot(glm::quat(-0.973045, -0.151482, 0.173103, -0.0164714));
        checkpoint27->SIL_X_SCALE = 1.02;
        checkpoint27->SIL_Y_SCALE = 1.02;
        checkpoint27->SIL_Z_SCALE = 1.02;
        checkpoint27->scaleModels();

        RectTrigger * checkpoint28 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 32, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint28);
        checkpoint28->setPos(-1769.83, 424.379, -384.326);
        checkpoint28->setRot(glm::quat(-0.939102, -0.172628, 0.297024, 0.00798817));
        checkpoint28->SIL_X_SCALE = 1.02;
        checkpoint28->SIL_Y_SCALE = 1.02;
        checkpoint28->SIL_Z_SCALE = 1.02;
        checkpoint28->scaleModels();


        RectTrigger * checkpoint29 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 33, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint29);
        checkpoint29->setPos(-1574.75, 510.071, -593.52);
        checkpoint29->setRot(glm::quat(-0.860209, -0.163924, 0.481085, 0.0415615));
        checkpoint29->SIL_X_SCALE = 1.02;
        checkpoint29->SIL_Y_SCALE = 1.02;
        checkpoint29->SIL_Z_SCALE = 1.02;
        checkpoint29->scaleModels();

        RectTrigger * checkpoint30 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 34, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint30);
        checkpoint30->setPos(-1160.61, 567.794, -726.67);
        checkpoint30->setRot(glm::quat(0.735707, 0.104639, -0.660131, -0.109606));
        checkpoint30->SIL_X_SCALE = 1.02;
        checkpoint30->SIL_Y_SCALE = 1.02;
        checkpoint30->SIL_Z_SCALE = 1.02;
        checkpoint30->scaleModels();

        RectTrigger * checkpoint31 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 35, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint31);
        checkpoint31->setPos(-590.038, 548.648, -613.014);
        checkpoint31->setRot(glm::quat(0.395436, 0.0996589, -0.906834, -0.106541));
        checkpoint31->SIL_X_SCALE = 1.02;
        checkpoint31->SIL_Y_SCALE = 1.02;
        checkpoint31->SIL_Z_SCALE = 1.02;
        checkpoint31->scaleModels();

        RectTrigger * checkpoint32 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 36, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint32);
        checkpoint32->setPos(-398.565, 525.1, -399.701);
        checkpoint32->setRot(glm::quat(0.222018, 0.11176, -0.966193, -0.0684792));
        checkpoint32->SIL_X_SCALE = 1.02;
        checkpoint32->SIL_Y_SCALE = 1.02;
        checkpoint32->SIL_Z_SCALE = 1.02;
        checkpoint32->scaleModels();

        RectTrigger * checkpoint33 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 37, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint33);
        checkpoint33->setPos(-351.648, 500.086, -14.6378);
        checkpoint33->setRot(glm::quat(-0.149905, 0.100667, -0.983546, -0.00571119));
        checkpoint33->SIL_X_SCALE = 1.02;
        checkpoint33->SIL_Y_SCALE = 1.02;
        checkpoint33->SIL_Z_SCALE = 1.02;
        checkpoint33->scaleModels();

        RectTrigger * checkpoint34 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 38, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint34);
        checkpoint34->setPos(-575, 487.986, 220.865);
        checkpoint34->setRot(glm::quat(-0.625581, 0.0875962, -0.775076, 0.0152323));
        checkpoint34->SIL_X_SCALE = 1.02;
        checkpoint34->SIL_Y_SCALE = 1.02;
        checkpoint34->SIL_Z_SCALE = 1.02;
        checkpoint34->scaleModels();

        RectTrigger * checkpoint35 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 39, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint35);
        checkpoint35->setPos(-969.637, 449.633, 249.517);
        checkpoint35->setRot(glm::quat(-0.831631, 0.0795034, -0.549608, 0.000418507));
        checkpoint35->SIL_X_SCALE = 1.02;
        checkpoint35->SIL_Y_SCALE = 1.02;
        checkpoint35->SIL_Z_SCALE = 1.02;
        checkpoint35->scaleModels();


        RectTrigger * checkpoint36 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 40, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint36);
        checkpoint36->setPos(-1224.66, 410.869, 27.1496);
        checkpoint36->setRot(glm::quat(-0.958508, 0.0381376, -0.282502, 0.000878059));
        checkpoint36->SIL_X_SCALE = 1.02;
        checkpoint36->SIL_Y_SCALE = 1.02;
        checkpoint36->SIL_Z_SCALE = 1.02;
        checkpoint36->scaleModels();

        RectTrigger * checkpoint37 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 41, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint37);
        checkpoint37->setPos(-1294.84, 403.196, -453.665);
        checkpoint37->setRot(glm::quat(-0.999998, 8.27058e-05, -0.00199661, -0.000206469));
        checkpoint37->SIL_X_SCALE = 1.02;
        checkpoint37->SIL_Y_SCALE = 1.02;
        checkpoint37->SIL_Z_SCALE = 1.02;
        checkpoint37->scaleModels();

        RectTrigger * checkpoint38 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 42, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint38);
        checkpoint38->setPos(-1150.55, 403.237, -967.154);
        checkpoint38->setRot(glm::quat(0.949936, -5.87319e-05, -0.312445, -9.68684e-05));
        checkpoint38->SIL_X_SCALE = 1.02;
        checkpoint38->SIL_Y_SCALE = 1.02;
        checkpoint38->SIL_Z_SCALE = 1.02;
        checkpoint38->scaleModels();

        RectTrigger * checkpoint39 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 43, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint39);
        checkpoint39->setPos(-976.342, 403.252, -1104.27);
        checkpoint39->setRot(glm::quat(0.896768, 3.07238e-05, -0.4425, -0.000181271));
        checkpoint39->SIL_X_SCALE = 1.02;
        checkpoint39->SIL_Y_SCALE = 1.02;
        checkpoint39->SIL_Z_SCALE = 1.02;
        checkpoint39->scaleModels();

        RectTrigger * checkpoint40 = new RectTrigger(myPhysics, "assets/models/track/green.png", 90., 20., 10., 44, glm::vec3(0, 0, 0), false);
        entities.push_back(checkpoint40);
        checkpoint40->setPos(-594.831, 403.279, -1192.47);
        checkpoint40->setRot(glm::quat(0.70856, 0.000152292, -0.70565, -0.00031293));
        checkpoint40->SIL_X_SCALE = 1.02;
        checkpoint40->SIL_Y_SCALE = 1.02;
        checkpoint40->SIL_Z_SCALE = 1.02;
        checkpoint40->scaleModels();


        trackNodes.push_back(finishLine);
        trackNodes.push_back(checkpoint0);
        trackNodes.push_back(checkpoint00);
        trackNodes.push_back(checkpoint1);
        trackNodes.push_back(checkpoint2);
        trackNodes.push_back(checkpoint3);
        trackNodes.push_back(checkpoint03);
        trackNodes.push_back(checkpoint4);
        trackNodes.push_back(checkpoint04);
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
        trackNodes.push_back(checkpoint18);
        trackNodes.push_back(checkpoint19);
        trackNodes.push_back(checkpoint20);
        trackNodes.push_back(checkpoint21);
        trackNodes.push_back(checkpoint22);
        trackNodes.push_back(checkpoint23);
        trackNodes.push_back(checkpoint24);
        trackNodes.push_back(checkpoint25);
        trackNodes.push_back(checkpoint26);
        trackNodes.push_back(checkpoint27);
        trackNodes.push_back(checkpoint28);
        trackNodes.push_back(checkpoint29);
        trackNodes.push_back(checkpoint30);
        trackNodes.push_back(checkpoint31);
        trackNodes.push_back(checkpoint32);
        trackNodes.push_back(checkpoint33);
        trackNodes.push_back(checkpoint34);
        trackNodes.push_back(checkpoint35);
        trackNodes.push_back(checkpoint36);
        trackNodes.push_back(checkpoint37);
        trackNodes.push_back(checkpoint38);
        trackNodes.push_back(checkpoint39);
        trackNodes.push_back(checkpoint40);

        for (int i = 0; i < numberOfPlayers; i++) {
            Car *car = new Car(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(CarColor(i)), nullptr, myPhysics, inputs[i], entities, jb, myTrack, trackNodes);
            cars.push_back(car);
            entities.push_back(car);
            car->engineSoundChannel = 100 + 5 * i;
            car->idleSoundChannel = 101 + 5 * i;
            car->hornSoundChannel = 102 + 5 * i;
        }

        for (int i = 0; i < (4 - numberOfPlayers); i++) {
            AICar *bot = new AICar(static_cast<CarColor>(cars.size()), "assets/models/car/testcar.obj", CarRenderInfo::getTex(CarColor(i + numberOfPlayers)), nullptr, myPhysics, entities, myTrack, trackNodes);
            cars.push_back(bot);
            entities.push_back(bot);
        }

        for (int i = 0; i < cars.size(); i++) {
            cars[i]->setPos(45.1907, 413.264, -1112.6 - (25 * i));
            cars[i]->setRot(glm::quat(-0.691184, -0.000337045, 0.722679, 0.000411464));
        }

        jb->loadMusic("assets/sound/dmw.mp3");
        break;
    }
}
    for (auto& e : entities) {
        if (e->canRender() || dynamic_cast<HookChain*>(e)) {
            renderables.push_back(static_cast<Renderable*>(e));
        }
    }

    jb->play();
        
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////
}

void GameState::endGame() {

    for (Entity* e : entities)
    {
        delete e;
    }
    cubes.clear();
    delete skyline;
    entities.clear();
    renderables.clear();
    cars.clear();
    trackNodes.clear();
    this->jb->stop(-1);
}
