#pragma once
//#include "GLUtil.h"
#include "model\Texture.h"
#include "text2D.h"
#include "../input/input.h"

class MenuRenderer {
    //Text rendering
    void MenuRenderer::initText();

    Text2D * blueText;
    Text2D * blackText;
    Text2D * redText;
    Text2D * whiteText;
    Text2D * blueTitle;
    Text2D * logo;

    int width;
    int height;

    bool playing = false;
    bool shouldClose = false;
    int selection = 0;
    int page = MAIN;
    Input * controller;

    void drawText();
    void drawDropShadowText(const char* string, Text2D* front, Text2D* back, int x, int y, int size, int offset);

public:
    enum page {
        MAIN = 0,
        SINGLE,
        MULTI,
        CREDITS,
        PAUSED, 
        LOADING
    };

    MenuRenderer();
    ~MenuRenderer();

    void postGLInit(); // call init functions after Window's created the OpenGL env.
    
    void drawScene();

    void drawMenu();
    void drawCredits();
    void drawLoadScreen();
    void drawPause();

    void drawCountDown(int time);

    void setDims(int width, int height);
    void registerController(Input * newCont);
    void updateMenu();

    bool getPlaying();
    void setPlaying(bool play);
    bool getShouldClose();
    void setPage(int newPage);
    int getPage();
    int getNumberOfItems(int page);
};
