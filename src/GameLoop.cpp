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
#include "GameState.h"

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
    //jb->play();

    // Set up input
   // std::unique_ptr<Input> input(new Input(0));

    Input * input = new Input(0);
    Input * input2 = new Input(1);
    Input * input3 = new Input(2);
    Input * input4 = new Input(3);

    vector<Input*> inputs;

    inputs.push_back(input);
    inputs.push_back(input2);
    inputs.push_back(input3);
    inputs.push_back(input4);

   // window->getRenderer()->getCam()->registerController(input.get());
   // window->getMenuRenderer()->registerController(input.get());


    window->getMenuRenderer()->registerController(input);


    StickListener stickListener;
    TriggerListener triggerListener;
    PhysicsManager * myPhysics = new PhysicsManager(&triggerListener, &stickListener);

    GameState gameState = GameState(inputs, myPhysics);


    // For each renderer in window, register a controller and a car for it if multiplayer is selected

#ifdef MENU_SKIP
    gameState.updateState(GameState::PLAYING);
    gameState.initGame();
    window->getRenderer()->getCam()->registerController(input);
    window->getRenderer()->getCam()->registerCar(gameState.cars[0]);

    glfwSetTime(0);
#endif // !MENU_SKIP



    while (!window->shouldClose())
    {
        switch (gameState.currentState) {
        case GameState::MENU:

            input->Update();
            window->drawMenu();

            if (window->getMenuRenderer()->getPlaying()) {
                gameState.updateState(GameState::PLAYING);
                int numOfPlayers = window->getMenuRenderer()->getNumOfPlayers();
                gameState.initGame(numOfPlayers);

                window->setSplitScreen(numOfPlayers, gameState.cars);

                for (int i = 0; i < numOfPlayers; i++ ) {
                    window->getRenderer(i)->getCam()->registerController(gameState.inputs[i]);
                    window->getRenderer(i)->getCam()->registerCar(gameState.cars[i]);
                }

/*
                window->getRenderer()->getCam()->registerController(input);
                window->getRenderer()->getCam()->registerCar(gameState.cars[0]);
*/
                // Update physics one time 
                // This will allow the wheels positions to be updated once and therfore will be rendered
                for (const auto& c : gameState.cars) {
                    c->stepForPhysics();
                }

                // Do the countdown
                int time = 3;
                float prevTime = clock();
                while (time > 0) {
                    float currentTime = clock();
                    if ((currentTime-prevTime) > 1000) {
                        time--;
                        prevTime = currentTime;
                    }
                    window->drawCountDown(gameState.entities, gameState.cars,time);
                    glfwSetTime(0);
                }
            }

            break;
        case GameState::PLAYING:

            for (Input * in : gameState.inputs) {
                in->Update();
            }
            for (const auto& c : gameState.cars) {
                if (c->pauseGame) {
                    gameState.updateState(GameState::PAUSED);
                    window->getMenuRenderer()->setPage(MenuRenderer::PAUSED);
                    window->getMenuRenderer()->setPlaying(false);
                    gameState.savedTime = glfwGetTime();
                }
            }

           // input->Update();
            for (const auto& c : gameState.cars) {
                c->stepForPhysics();
                c->update();
            }
            gameState.myPhysics->stepPhysics();

            // This Keeps the " GO " message up for a period of time after the race starts
            if (glfwGetTime() > 2) {
                window->draw(gameState.entities, gameState.cars);
            }
            else {
                window->drawCountDown(gameState.entities, gameState.cars, 0);
            }

            break;
        case GameState::PAUSED:
            input->Update();
            window->drawMenu();

            //Resume game
            if (window->getMenuRenderer()->getPlaying()) {
                gameState.updateState(GameState::PLAYING);

                for (const auto& c : gameState.cars) {
                    c->pauseGame = false;
                }
                glfwSetTime(gameState.savedTime);
            }

            //Back to main menu
            if (window->getMenuRenderer()->getPage() == MenuRenderer::page::MAIN) {
                gameState.updateState(GameState::MENU);
                for (const auto& c : gameState.cars) {
                    c->pauseGame = false;
                }
                gameState.endGame();
            }

            // if menurenderer on main menu again, set gamestate to 
            break;
        }
    }   
	delete gameState.myPhysics;
	return 0;
}