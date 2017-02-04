#pragma once
#include <memory>
#include "renderer.h"
#include <GLFW/glfw3.h>
#include "../entity/Renderable.h"

class Window {
    static const int MAX_RENDERERS = 4;

    // SOON, MY DARLINGS
    // std::unique_ptr<GLFWwindow> window; 
    //std::unique_ptr<Renderer> renderers[MAX_RENDERERS];
    GLFWwindow* window = 0;
    static Renderer* renderer;
    static bool done_init;

    static glm::vec2 mousePos;
    static bool mousePressed;

    int initGLFW();
    // --------------------------------------------------------------------------
    // GLFW callback functions
    // TODO: Move these elsewhere - maybe hook in with input or an inputListener class

    // reports GLFW errors
    static void ErrorCallback(int error, const char* description);

    // handles input events
    // TODO: stop using these callbacks to get user input; instead, use a proper input class
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mousePosCallback(GLFWwindow* window, double xpos, double ypos);
    static void resizeCallback(GLFWwindow* window, int width, int height);

public:
    Window();
    ~Window();

    void Window::draw(const std::vector<Entity*>& ents);
    bool Window::shouldClose();
};