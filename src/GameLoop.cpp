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
#include "entity/Skyline.h"
#include "GameState.h"

#include "Jukebox.h"
#include <ctime>
#include "util/ConfigParser.h"
#include "GameLoop.h"

using namespace std;

int main(int argc, const char* argv[])
{
    std::unique_ptr<Window> window(new Window(1280, 720));

	//Music
	Jukebox *jb = new Jukebox();
    jb->setup();
    jb->play();
    window->getMenuRenderer()->registerJukebox(jb);

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


//   window->getMenuRenderer()->registerController(input);

    window->getMenuRenderer()->registerControllers(input);
    window->getMenuRenderer()->registerControllers(input2);
    window->getMenuRenderer()->registerControllers(input3);
    window->getMenuRenderer()->registerControllers(input4);

    StickListener stickListener;
    TriggerListener triggerListener;
    PhysicsManager * myPhysics = new PhysicsManager(&triggerListener, &stickListener);

    //GameState gameState = GameState(inputs, myPhysics);

    // For each renderer in window, register a controller and a car for it if multiplayer is selected



    GameState gameState = GameState(inputs, myPhysics, jb);

#ifdef MENU_SKIP
    gameState.updateState(GameState::PLAYING);
    gameState.initGame(1);
    window->getRenderer()->getCam()->registerController(input);
    window->getRenderer()->getCam()->registerCar(gameState.cars[0]);

    glfwSetTime(0);
#endif // !MENU_SKIP

    while (!window->shouldClose())
    {
        switch (gameState.currentState) {
        case GameState::MENU:

            //input->Update();
            for (Input * in : gameState.inputs) {
                in->Update();
            }

            window->drawMenu();

            if (window->getMenuRenderer()->getPlaying()) {
                gameState.updateState(GameState::PLAYING);
                int numOfPlayers = window->getMenuRenderer()->getNumOfPlayers();
                window->setMMSize(window->getMenuRenderer()->getTrackSelection());
                gameState.initGame(numOfPlayers, window->getMenuRenderer()->getTrackSelection());

                window->setSplitScreen(numOfPlayers, gameState.cars);

                window->initSkyboxes(window->getMenuRenderer()->getTrackSelection());
                for (int i = 0; i < numOfPlayers; i++) {
                    window->getRenderer(i)->getCam()->registerController(gameState.inputs[i]);
                    window->getRenderer(i)->getCam()->registerCar(gameState.cars[i]);
                }

                /*
                                window->getRenderer()->getCam()->registerController(input);
                                window->getRenderer()->getCam()->registerCar(gameState.cars[0]);
                */
                // Update physics one time 
                // This will allow the wheels positions to be updated once and therfore will be rendered
                #pragma omp parallel for
                for (auto &it = gameState.cars.begin(); it < gameState.cars.end(); it++) {
                    Car* c = *it;
                    c->stepForPhysics();
                }

                // Do the countdown
                int time = 3;
                float prevTime = clock();

                jb->playEffect(Jukebox::soundEffects::menumove);
                window->drawCountDown(gameState.renderables, gameState.cars, gameState.cubes, time, false);
                gameState.skyline = new Skyline(window->getMMSize(), window->getMiniMapBG(), 50, gameState.cubes, input, window->getMenuRenderer()->getTrackSelection());
                while (time > 0) {
                    float currentTime = clock();
                    if ((currentTime - prevTime) > 1000) {
                        time--;
                        prevTime = currentTime;
                        if (time!=0)
                            jb->playEffect(Jukebox::soundEffects::menumove);
                    }
                    gameState.skyline->update();
                    window->drawCountDown(gameState.renderables, gameState.cars, gameState.cubes, time);
                    glfwSetTime(0);
                }

                jb->playEffect(Jukebox::soundEffects::horn);
            }

            break;
        case GameState::PLAYING:
            gameState.skyline->update();
            for (Input * in : gameState.inputs) {
                in->Update();
            }
            for (const auto& c : gameState.cars) {
                if (c->pauseGame) {
                    jb->playEffect(Jukebox::menuselect);
                    gameState.updateState(GameState::PAUSED);
                    window->getMenuRenderer()->setPage(MenuRenderer::PAUSED);
                    window->getMenuRenderer()->setPlaying(false);
                    gameState.savedTime = glfwGetTime();
                }
            }

            {
            vector<Car*> sortcars;
            for (Car * c : gameState.cars)
            {
                if (!c->doneRace)
                {
                    sortcars.push_back(c);
                    //////// Calc positions
                    float dist = glm::length(c->nodes.at(c->partoflap % c->nodes.size())->getPos() - c->getPos());
                    float maxdist = glm::length(c->nodes.at(c->partoflap %c->nodes.size())->getPos() - c->nodes.at(((c->partoflap) - 1) % c->nodes.size())->getPos());
                    c->score = c->lap * 100 + c->partoflap + (1.0f - dist / maxdist);
                }
            }
                
            #pragma omp parallel for
            for (auto &it = gameState.cars.begin(); it < gameState.cars.end(); it++) {
                Car* c = *it;
                c->stepForPhysics();
                c->update();
            }

            sortcars = Car::sortByScore(sortcars);
            for (int i = 0; i < sortcars.size(); i++)
                sortcars.at(i)->rank = 4-i;
            }
            gameState.myPhysics->stepPhysics();

            // This Keeps the " GO " message up for a period of time after the race starts
            // It will also display the results of the screen as an overlay omce everyone has completed the race
            if (glfwGetTime() < 2) {
                window->drawCountDown(gameState.renderables, gameState.cars, gameState.cubes, 0);
            }
            else if (gameState.isRaceComplete()==true) {
                // Do the countdown
                if (gameState.countDownLength == gameState.countDownMax) {
                    gameState.savedTime = clock();
                    gameState.countDownLength--;
                }

                float currentTime = clock();
                if ((currentTime - gameState.savedTime) > 1000) {
                    gameState.countDownLength--;
                    gameState.savedTime = currentTime;
                    if (gameState.countDownLength != 0)
                        jb->playEffect(Jukebox::soundEffects::menumove);
                }
                window->drawFinalScores(gameState.renderables, gameState.cars, gameState.cubes, gameState.countDownLength);

                if (gameState.countDownLength == 0) {
                    window->getMenuRenderer()->setPage(MenuRenderer::MAIN);
                    window->getMenuRenderer()->setPlaying(false);
                    jb->playEffect(Jukebox::menuselect);
                    jb->loadMusic("assets/sound/shootingstars.mp3");
                    jb->play();
                    gameState.updateState(GameState::MENU);
                    for (const auto& c : gameState.cars) {
                        c->pauseGame = false;
                    }
                    gameState.countDownLength = gameState.countDownMax;
                    gameState.endGame();
                }
            }
            else {
                window->draw(gameState.renderables, gameState.cars, gameState.cubes);
            }
            break;
        case GameState::PAUSED:
            //input->Update();
            
            for (Input * in : gameState.inputs) {
                in->Update();
            }
            window->drawMenu();

            //Resume game
            if (window->getMenuRenderer()->getPlaying()) {
                jb->playEffect(Jukebox::menuselect);
                gameState.updateState(GameState::PLAYING);

                for (const auto& c : gameState.cars) {
                    c->pauseGame = false;
                }
                glfwSetTime(gameState.savedTime);
            }

            //Back to main menu
            if (window->getMenuRenderer()->getPage() == MenuRenderer::page::MAIN) {
                jb->playEffect(Jukebox::menuselect);
                jb->loadMusic("assets/sound/shootingstars.mp3");
                jb->play();
                gameState.updateState(GameState::MENU);
                for (const auto& c : gameState.cars) {
                    c->pauseGame = false;
                }
                gameState.countDownLength = gameState.countDownMax;
                gameState.endGame();
            }

            // if menurenderer on main menu again, set gamestate to 
            break;
        }
    }
    // Check that we haven't already ended the game elsewhere
    if (gameState.entities.size() > 0) {
        gameState.endGame();
    }
    delete jb;
	delete gameState.myPhysics;

	return 0;
}

