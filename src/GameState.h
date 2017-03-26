#pragma once
#include "renderer\Window.h"
#include "input\input.h"
#include "physics/PhysicsManager.h"

#include "Jukebox.h"

class GameState {
   
public:
    enum State
    {
        PLAYING = 0,
        MENU,
        PAUSED
    };

    Input * input;
    PhysicsManager * myPhysics;
    Jukebox * jb;

    std::vector<Entity*> entities;
    std::vector<Car*> cars;
    std::vector<RectTrigger*> trackNodes;

    float savedTime = 0.f;

    GameState();
    GameState(Input * newInput, PhysicsManager * myPhysics, Jukebox * music);

    ~GameState();

    int currentState = MENU;

    void GameState::updateState(State state);
    void initGame();
    void endGame();
};
