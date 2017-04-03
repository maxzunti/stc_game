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
    float trackScale = 50.0f;

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

    GameState();
    GameState(Input * newInput, PhysicsManager * myPhysics);
    GameState(std::vector<Input*> newInputs, PhysicsManager * myPhysics, Jukebox * music);
    GameState(Input * newInput, PhysicsManager * myPhysics, Jukebox * music);
    ~GameState();

    int currentState = MENU;

    void GameState::updateState(State state);
    void initGame(int numberOfPlayers);
    void endGame();
};
