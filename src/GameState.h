#pragma once
#include "renderer\Window.h"
#include "input\input.h"
#include "physics/PhysicsManager.h"

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

    std::vector<Entity*> entities;
    std::vector<Car*> cars;

    float savedTime = 0.f;

    GameState();
    GameState(Input * newInput, PhysicsManager * myPhysics);

    ~GameState();

    int currentState = MENU;

    void GameState::updateState(State state);
    void initGame();
    void endGame();
};
