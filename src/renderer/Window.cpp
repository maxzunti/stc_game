#include "Window.h"
#include <iostream>
/*
// Do we need this? seems like it should go in GLUtil, if anything.
// (Actually, seems like glm should already do this sort of this for us)
mat4 rotateAboutAxis(vec3 axis, float radians)
{
mat4 matrix;

matrix[0][0] = cos(radians) + axis.x*axis.x*(1 - cos(radians));
matrix[1][0] = axis.x*axis.y*(1 - cos(radians)) - axis.z*sin(radians);
matrix[2][0] = axis.x*axis.z*(1 - cos(radians)) + axis.y*sin(radians);

matrix[0][1] = axis.y*axis.x*(1 - cos(radians)) + axis.z*sin(radians);
matrix[1][1] = cos(radians) + axis.y*axis.y*(1 - cos(radians));
matrix[2][1] = axis.y*axis.z*(1 - cos(radians)) - axis.x*sin(radians);

matrix[0][2] = axis.z*axis.x*(1 - cos(radians)) - axis.y*sin(radians);
matrix[1][2] = axis.z*axis.y*(1 - cos(radians)) + axis.x*sin(radians);
matrix[2][2] = cos(radians) + axis.z*axis.z*(1 - cos(radians));

return matrix;
}*/

Renderer* Window::renderer = new Renderer(0, glm::vec3(200, 400, 200), glm::vec3(-400, -500, -100));
glm::vec2 Window::mousePos(0, 0);
bool Window::mousePressed = false;
bool Window::done_init = false;

Window::Window() {
    if (!done_init) {
        initGLFW();

        if (!initGL()) {
            std::cout << "Error: OpenGL initialization failed" << std::endl;
        }
        else {
            done_init = true;
        }
    }
    renderer->initSkybox();
    renderer->loadFrameBuffers();
    //renderer->initText("assets/textures/font.png");
}

Window::~Window() {
    // clean up allocated resources before exit
    glfwDestroyWindow(window);
    glfwTerminate();
}

// initialize the GLFW windowing system
int Window::initGLFW() {
    if (!glfwInit()) {
        std::cout << "ERROR: GLFW failed to initilize, TERMINATING" << std::endl;
        return -1;
    }
    glfwSetErrorCallback(this->ErrorCallback);

    // attempt to create a window with an OpenGL 4.1 core profile context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1024, 768, "GRAV GRAPPLERS", 0, 0);
    if (!window) {
        std::cout << "Program failed to create GLFW window, TERMINATING" << std::endl;
        glfwTerminate();
    }
    
    // set keyboard callback function and make our context current (active)
    glfwSetKeyCallback(window, this->keyCallback);
    glfwSetMouseButtonCallback(window, this->mouseButtonCallback);
    glfwSetCursorPosCallback(window, this->mousePosCallback);
    glfwSetWindowSizeCallback(window, this->resizeCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    return 0;
}

void Window::draw(const std::vector<Entity*>& ents) {
    renderer->drawScene(ents);
    // scene is rendered to the back buffer, so swap to front for display
    glfwSwapBuffers(window);

    // sleep until next event before drawing again
    glfwPollEvents();
}

bool Window::shouldClose()
{
    if (glfwWindowShouldClose(window))
        return true;
    else return false;
}


// --------------------------------------------------------------------------
// GLFW callback functions
// TODO: Move these elsewhere - maybe hook in with input or an inputListener class

using namespace glm;

// reports GLFW errors
void Window::ErrorCallback(int error, const char* description)
{
    std::cout << "GLFW ERROR " << error << ":" << std::endl;
    std::cout << description << std::endl;
}

// handles keyboard input events
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Camera* cam = renderer->getCam();
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    float move = 0.05f;

    if (key == GLFW_KEY_W) {
        if (cam->zoom < -0.5) {
            cam->pos += cam->dir*move;
            cam->zoom += move;
        }
        else {
            cam->zoom = -0.5f;
        }
    }
    else if (key == GLFW_KEY_S) {
        if (cam->zoom > -10) {
            cam->pos -= cam->dir*move;
            cam->zoom -= move;
        }
        else {
            cam->zoom = -10.0f;
        }
    }
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if ((action == GLFW_PRESS) || (action == GLFW_RELEASE))
        mousePressed = !mousePressed;
}

void Window::mousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Camera* cam = renderer->getCam();
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    vec2 newPos = vec2(xpos / (double)vp[2], -ypos / (double)vp[3])*2.f - vec2(1.f);

    vec2 diff = newPos - mousePos;
    if (mousePressed) {
        cam->rotateAroundCenter(-diff.x, diff.y, vec3(0, 0, 0));
    }
    mousePos = newPos;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);

    glViewport(0, 0, width, height);
}

Renderer* Window::getRenderer() { return renderer;  }