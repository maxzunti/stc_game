#include "Window.h"
#include <iostream>

//Renderer* Window::renderer = new Renderer(0);

MenuRenderer* Window::menuRenderer = new MenuRenderer();
glm::vec2 Window::mousePos(0, 0);
bool Window::mousePressed = false;
bool Window::done_init = false;
bool Window::update = false;
int Window::width = 1280;
int Window::height = 720;

Window::Window(int width, int height) {
    this->width = width;
    this->height = height;

    if (!done_init) {
        initGLFW();

        if (!initGL()) {
            std::cout << "Error: OpenGL initialization failed" << std::endl;
        }
        else {
            done_init = true;
        }
    }
    Renderer * r = new Renderer(0);
    renderWindowData rwd;
    rwd.height = height;
    rwd.width = width;
    rwd.xPos = 0;
    rwd.yPos = 0;
    r->setDims(rwd);
    r->postGLInit();
    r->initFrameBuffer();
    renderers.push_back(r);
   // renderer->setDims(width, height);
    //renderer->postGLInit();
    menuRenderer->setDims(width, height);
    menuRenderer->postGLInit();
}

Window::~Window() {
    // clean up allocated resources before exit
    glfwDestroyWindow(window);
    glfwTerminate();
    for (auto r : renderers) {
        delete r;
    }
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
    glfwWindowHint(GLFW_SAMPLES, 8);
    window = glfwCreateWindow(width, height, "GRAV GRAPPLERS", 0, 0);
    if (!window) {
        std::cout << "Program failed to create GLFW window, TERMINATING" << std::endl;
        glfwTerminate();
    }
    
    // set keyboard callback function and make our context current (active)
    glfwSetWindowSizeCallback(window, this->resizeCallback);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    return 0;
}

void Window::draw(const std::vector<Renderable*>& ents, const std::vector<Car*>& cars, const std::vector<Renderable*>& cubes) {
    SSParams params = getSSParams(nps);
    if (update) {
        for (int i = 0; i < renderers.size(); i++) {
            renderWindowData rwd;
            rwd.height = params.screenPos[i].height;
            rwd.width = params.screenPos[i].width;
            rwd.xPos = params.screenPos[i].xPos;
            rwd.yPos = params.screenPos[i].yPos;
            renderers[i]->setDims(rwd);
        }
        update = false;
    }

    renderers[0]->renderShadowMap(ents);
    renderers[0]->drawSkylineShadows(cubes);
    for (auto r : renderers) {
        r->draw(ents, cars, cubes);
    }

    renderers[0]->renderMiniMap(ents, cars, 1300, renderers[0]->getMMSize(), params.mapPos[0]- (renderers[0]->getMMSize() /2), params.mapPos[1] - (renderers[0]->getMMSize() / 2), width, height, 0.7);
    
    // scene is rendered to the back buffer, so swap to front for display
    glfwSwapBuffers(window);

    // sleep until next event before drawing again
    glfwPollEvents();
}

void Window::drawMMOnly(const std::vector<Renderable*>& ents, const std::vector<Car*>& cars) {
    SSParams params = getSSParams(nps);
    if (update) {
        for (int i = 0; i < renderers.size(); i++) {
            renderWindowData rwd;
            rwd.height = params.screenPos[i].height;
            rwd.width = params.screenPos[i].width;
            rwd.xPos = params.screenPos[i].xPos;
            rwd.yPos = params.screenPos[i].yPos;
            renderers[i]->setDims(rwd);
        }
        update = false;
    }
    renderers[0]->renderMiniMap(ents, cars, 1300, renderers[0]->getMMSize(), params.mapPos[0] - (renderers[0]->getMMSize() / 2), params.mapPos[1] - (renderers[0]->getMMSize() / 2), width, height, 0.7);
}

void Window::drawMenu() {
    if (menuRenderer->getShouldClose()) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    menuRenderer->drawScene();
   
    // scene is rendered to the back buffer, so swap to front for display
    glfwSwapBuffers(window);

    // sleep until next event before drawing again
    glfwPollEvents();
}

void Window::drawCountDown(const std::vector<Renderable*>& ents, const std::vector<Car*>& cars, const std::vector<Renderable*>& cubes, int time, bool swapBuffer) {
    SSParams params = getSSParams(nps);
    if (update) {
        for (int i = 0; i < renderers.size(); i ++) {
            renderWindowData rwd;
            rwd.height = params.screenPos[i].height;
            rwd.width = params.screenPos[i].width;
            rwd.xPos = params.screenPos[i].xPos;
            rwd.yPos = params.screenPos[i].yPos;
            renderers[i]->setDims(rwd);
        }
        update = false;
    }

    renderers[0]->renderShadowMap(ents);
    renderers[0]->drawSkylineShadows(cubes);
    for (auto r : renderers) {
        r->draw(ents, cars, cubes);
        r->drawCountDown(time);
    }

    renderers[0]->renderMiniMap(ents, cars, 1300, renderers[0]->getMMSize(), params.mapPos[0] - (renderers[0]->getMMSize() / 2), params.mapPos[1] - (renderers[0]->getMMSize() / 2), width, height, 0.7);

    // scene is rendered to the back buffer, so swap to front for display
    if (swapBuffer) {
        glfwSwapBuffers(window);
    }

    // sleep until next event before drawing again
    glfwPollEvents();
}

// Call only after we've done at least one regular draw call
GLuint& Window::getMiniMapBG() {
    return renderers[0]->getMiniMapBG();
}

int Window::getMMSize() {
    return renderers[0]->getMMSize();
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

void Window::resizeCallback(GLFWwindow* window, int nWidth, int nHeight)
{
    int vp[4];
    glGetIntegerv(GL_VIEWPORT, vp);
    width = nWidth;
    height = nHeight;
    update = true;
    menuRenderer->setDims(width, height);
}

void Window::setSplitScreen(int numPlayers, const std::vector<Car*>& cars) {
    nps = numPlayers;
    assert(cars.size() == numPlayers); // this check implies that numPlayers doesn't necessarily need to exist at all:
    // however, it forces whoever's managing the window to make sure that "cars" has been managed appropriately
    // also, AICars don't get a screen: MAKE SURE THEYRE PUSHED BACK TO CARS LAST

    if (numPlayers > Window::MAX_RENDERERS || numPlayers < 1) {
        std::cout << "Error: invalid number of players emtered" << std::endl;
        return;
    }

    // We need to trim the number of renderers according to the number of players
    // The two should be equal
    for (int i = renderers.size() - 1; i >= numPlayers; i--) {
        renderers.pop_back();
    }

    SSParams sParams = getSSParams(numPlayers);
    assert(sParams.screenPos.size() == numPlayers); // this better be true
    if (renderers.size() <= numPlayers) {
        for (int i = 0; i < renderers.size(); i++) { // update existing renderers
            renderers.at(i)->setDims(sParams.screenPos.at(i));
        }
        for (int i = renderers.size(); i < numPlayers; i++) { // create new ones
            Renderer * r = new Renderer(i);
            r->postGLInit();
            r->setDims(sParams.screenPos.at(i));
            r->getCam()->registerCar(cars.at(i));
            renderers.push_back(r);
        }
    }
    //else { // delete excess renderers from back
    //    for (int i = renderers.size() - 1; i >= numPlayers; i--) {
    //        renderers.pop_back();
    //    }
    //}
    //std::cout << "rs size = " << renderers.size() << std::endl;
}

Renderer* Window::getRenderer(int index) { return renderers.at(index); }

MenuRenderer * Window::getMenuRenderer()
{
    return menuRenderer;
}

SSParams Window::getSSParams(int numPlayers) {
    SSParams params;
    switch (numPlayers) {
    case 1: {
        renderWindowData w1;
        w1.xPos = 0;
        w1.yPos = 0;
        w1.width = width;
        w1.height = height;
        params.screenPos.push_back(w1); // xPos, yPos, width, height

        params.mapPos[0] = width - height/4;
        params.mapPos[1] = height/2.f;
        break;
    }
    case 2: {
        renderWindowData w1;
        w1.xPos = 0;
        w1.yPos = 0;
        w1.width = width / 2;
        w1.height = height;
        params.screenPos.push_back(w1); // xPos, yPos, width, height
        renderWindowData w2;
        w2.xPos = width / 2;
        w2.yPos = 0;
        w2.width = width / 2;
        w2.height = height;
        params.screenPos.push_back(w2); // xPos, yPos, width, height

        params.mapPos[0] = (width / 2.f);
        params.mapPos[1] = (height/2.f);
        break;
    }
        //TODO:: get these working
    case 3: {
        //params.screenPos.push_back({ 0, 0, width, height }); // xPos, yPos, width, height
        //params.mapPos[0] = width;
        //params.mapPos[1] = height;

        renderWindowData w1;
        w1.xPos = 0;
        w1.yPos = 0;
        w1.width = width / 2;
        w1.height = height;
        params.screenPos.push_back(w1); // xPos, yPos, width, height

        renderWindowData w2;
        w2.xPos = width / 2;
        w2.yPos = height / 2;
        w2.width = width / 2;
        w2.height = height / 2;
        params.screenPos.push_back(w2); // xPos, yPos, width, height

        renderWindowData w3;
        w3.xPos = width / 2;
        w3.yPos = 0;
        w3.width = width / 2;
        w3.height = height / 2;
        params.screenPos.push_back(w3); // xPos, yPos, width, height

        params.mapPos[0] = (width / 2.f);
        params.mapPos[1] = (height / 2.f);
        break;
    }
    case 4: {
        renderWindowData w1;
        w1.xPos = 0;
        w1.yPos = height / 2;
        w1.width = width / 2;
        w1.height = height / 2;
        params.screenPos.push_back(w1); // xPos, yPos, width, height

        renderWindowData w2;
        w2.xPos = width / 2;
        w2.yPos = height / 2;
        w2.width = width / 2;
        w2.height = height / 2;
        params.screenPos.push_back(w2); // xPos, yPos, width, height

        renderWindowData w3;
        w3.xPos = 0;
        w3.yPos = 0;
        w3.width = width / 2;
        w3.height = height / 2;
        params.screenPos.push_back(w3); // xPos, yPos, width, height

        renderWindowData w4;
        w4.xPos = width / 2;
        w4.yPos = 0;
        w4.width = width / 2;
        w4.height = height / 2;
        params.screenPos.push_back(w4); // xPos, yPos, width, height


        params.mapPos[0] = (width / 2.f);
        params.mapPos[1] = (height / 2.f);
        break;
    }
    }
    return params;
}

