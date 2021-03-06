#pragma once
#include "renderer\Window.h"
#include "input\input.h"
#include "physics/PhysicsManager.h"
#include "entity\Track.h"
#include "entity/Skyline.h"

#include "Jukebox.h"

class GameState {
   
public:
    enum State
    {
        PLAYING = 0,
        MENU,
        PAUSED
    };
    float trackScale = 250.0f; // map 1 is 50

    Input * input;
    PhysicsManager * myPhysics;
    Jukebox * jb;
    Skyline* skyline;

    std::vector<Input*> inputs;
    std::vector<Entity*> entities;
    std::vector<Renderable*> renderables;
    std::vector<Renderable*> cubes;
    std::vector<Car*> cars;
    std::vector<RectTrigger*> trackNodes;
    Track* myTrack;

    float savedTime = 0.f;
    // Change countdown max to change the countdown time
    int countDownLength;
    int countDownMax = 11;

    GameState();
    GameState(Input * newInput, PhysicsManager * myPhysics);
    GameState(std::vector<Input*> newInputs, PhysicsManager * myPhysics, Jukebox * music);
    GameState(Input * newInput, PhysicsManager * myPhysics, Jukebox * music);
    ~GameState();

    int currentState = MENU;

    void GameState::updateState(State state);
    bool isRaceComplete();
    void initGame(int numberOfPlayers, int selectedTrack);
    void endGame();
};
