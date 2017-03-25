#pragma once
#include <memory>
#include "renderer.h"
#include <GLFW/glfw3.h>
#include "../entity/Renderable.h"
#include "MenuRenderer.h"

struct SSParams {
    std::vector<renderWindowData> screenPos; // individual screen positions for splitscreen
    int mapPos[2];                 // // xPos, yPos (renderer handles width, height, etc.)
};

class Window {
    static const int MAX_RENDERERS = 4;

    // SOON, MY DARLINGS
    // std::unique_ptr<GLFWwindow> window; 
    //std::unique_ptr<Renderer> renderers[MAX_RENDERERS];
    GLFWwindow* window = 0;
    // Renderer* renderer;
    std::vector<Renderer*> renderers;
    static MenuRenderer* menuRenderer;
    static bool done_init;

    static glm::vec2 mousePos;
    static bool mousePressed;

    int initGLFW();
    // --------------------------------------------------------------------------
    // GLFW callback functions
    // TODO: Move these elsewhere - maybe hook in with input or an inputListener class

    // reports GLFW errors
    static void ErrorCallback(int error, const char* description);

    static void resizeCallback(GLFWwindow* window, int width, int height);

    static int width;
    static int height;
    static bool update;
    static SSParams getSSParams(int numPlayers);
    int nps = 0;

public:
    Window(int width, int height);
    ~Window();

    void Window::draw(const std::vector<Entity*>& ents, const std::vector<Car*>& cars);
    void drawMenu();
    void drawCountDown(const std::vector<Entity*>& ents, const std::vector<Car*>& cars, int time);
    bool Window::shouldClose();
    void setSplitScreen(int numPlayers, const std::vector<Car*>& cars);
    Renderer* getRenderer(int index = 0); // delete once renderer is non-static
    MenuRenderer* getMenuRenderer();
};