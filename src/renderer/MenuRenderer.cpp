#include "MenuRenderer.h"
#include <iostream>
#include <string.h>

using namespace std;
using namespace glm;

bool pressed = false;
bool aPressed = false;

MenuRenderer::MenuRenderer() {
}

MenuRenderer::~MenuRenderer() {

}

void MenuRenderer::postGLInit() {
    initText();
}

void MenuRenderer::initText() {
    blueText = new Text2D("assets/textures/blue_gg_font.png");
    blackText = new Text2D("assets/textures/black_gg_font.png");
    //redText = new Text2D("assets/textures/red_gg_font.png");
    whiteText = new Text2D("assets/textures/white_gg_font.png");
    blueTitle = new Text2D("assets/logo/gg_blue.png");
    logo = new Text2D("assets/models/plane/stcTrans.png");
}


void MenuRenderer::drawScene()
{
    glDisable(GL_CULL_FACE);

    glViewport(0, 0, width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right

    //float fovy, float aspect, float zNear, float zFar
    glClearColor(0.1f, 0.1f, 0.1f, 0.1f);		//Color to clear the screen with (R, G, B, Alpha)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		//Clear color and depth buffers (Haven't covered yet)

    // Will need to move this to a different location
    updateMenu();

    // (Likely) use a new shader program

    glUseProgram(shader[SHADER::TEXT]);
        
    drawText();
}

void MenuRenderer::drawText() {
    // Draw text here
    if (page == MAIN) {
        drawMenu();
    }
    else if(page == MULTI){
        drawMultiplayerMenu();
    }
    else if (page == CREDITS) {
        drawCredits();
    }
    else if (page == LOADING) {
        drawLoadScreen();
    }
    else if (page == PAUSED) {
        drawPause();
    }
}

void MenuRenderer::drawMenu() {
    int xPlacement;
    int yPlacement;
    int ds_offset = 3;
    int size = 50* (height/(720.f));

    // Show the title at the top of the menu
    blueTitle->drawTexture(0, height*(2.f / 3.f), height, height / 3.f, width, height, 1.f, false);

    logo->drawTexture(0, 0, width / 5, height / 5, width, height, 1.f, false);
    //Position - insert real position info here
    // X pos calculation is done by dividing the size by 2 (the kerning factor in the function halves it)
    // and then multiplying this by half of the string length

    //Singleplayer Option
    char spText[512];
    sprintf(spText, "SINGLE PLAYER");

    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(spText) / 2.f);
    yPlacement = this->height / 2.f;

    if (selection == 0) {
        drawDropShadowText(spText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(spText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    //Multi-player Option
    char mpText[512];
    sprintf(mpText, "MULTIPLAYER");

    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(mpText) / 2.f);
    yPlacement = (this->height / 2.f) - size;

    // drawDropShadowText(mpText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    if (selection == 1) {
        drawDropShadowText(mpText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(mpText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    //Credits Option
    char cText[512];
    sprintf(cText, "CREDITS");

    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(cText) / 2.f);
    yPlacement = (this->height / 2.f) - (2*size);

    //drawDropShadowText(cText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    if (selection == 2) {
        drawDropShadowText(cText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(cText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    //Quit Option
    char qText[512];
    sprintf(qText, "QUIT GAME");

    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(qText) / 2.f);
    yPlacement = (this->height / 2.f) - (4*size);

    //drawDropShadowText(qText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    if (selection == 3) {
        drawDropShadowText(qText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(qText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
}

void MenuRenderer::drawMultiplayerMenu() {
    int xPlacement;
    int yPlacement;
    int ds_offset = 3;
    int size = 75 * (height / (720.f));

    // Show the title at the top of the menu
  //  blueTitle->drawTexture(0, height*(2.f / 3.f), height, height / 3.f, width, height, 1.f, false);


    char sText[512];
    sprintf(sText, "SELECT NUMBER OF PLAYERS:");

    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(sText) / 2.f);
    yPlacement = (this->height * (3.f / 4.f));

    drawDropShadowText(sText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
   
   
    // Set the size of the Numbers to be much larger so that they stand out
    size = 200 * (height / (720.f));

    //Position - insert real position info here
    // X pos calculation is done by dividing the size by 2 (the kerning factor in the function halves it)
    // and then multiplying this by half of the string length

    char spText[512];
    sprintf(spText, "1");

   // xPlacement = (this->width / 4.f)/* - (size / 2.f) * (4)*/;
    xPlacement = 0;

    yPlacement = this->height / 2.f - size;

    if (selection == 0) {
        drawDropShadowText(spText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(spText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }

    char mpText[512];
    sprintf(mpText, "2");

    xPlacement = (this->width / 4.f);
    yPlacement = (this->height / 2.f) - size;

    if (selection == 1) {
        drawDropShadowText(mpText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(mpText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }

    char cText[512];
    sprintf(cText, "3");

    xPlacement = (this->width / 2.f);
    yPlacement = (this->height / 2.f) - size;

    if (selection == 2) {
        drawDropShadowText(cText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(cText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }

    char qText[512];
    sprintf(qText, "4");

    xPlacement = (this->width / 4.f) + (this->width / 2.f);
    yPlacement = (this->height / 2.f) - size;

    if (selection == 3) {
        drawDropShadowText(qText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(qText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
}


void MenuRenderer::drawCredits() {
    blueTitle->drawTexture(0, height*(2.f / 3.f), height, height / 3.f, width, height, 1.f, false);
    int xPlacement = 30;
    int yPlacement = this->height - 100;
    int ds_offset = 3;
    int size = 80 * (height / (720.f));

    //Position - insert real position info here
    // X pos calculation is done by dividing the size by 2 (the kerning factor in the function halves it)
    // and then multiplying this by half of the string length

    char spText[512];
    sprintf(spText, "MATTHEW BAUMAN");

    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(spText) / 2.f);
    yPlacement = this->height / 2.f;

    drawDropShadowText(spText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);

    char mpText[512];
    sprintf(mpText, "MAX ZUNTI");

    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(mpText) / 2.f);
    yPlacement = (this->height / 2.f) - size;
   
    drawDropShadowText(mpText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);

    char cText[512];
    sprintf(cText, "DAVID WILKINSON");

    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(cText) / 2.f);
    yPlacement = (this->height / 2.f) - size*2;

    drawDropShadowText(cText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
 
    char qText[512];
    sprintf(qText, "STEVEN SIEU");

    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(qText) / 2.f);
    yPlacement = (this->height / 2.f) - size*3;

    drawDropShadowText(qText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
 
}

void MenuRenderer::drawLoadScreen()
{
    int xPlacement;
    int yPlacement;
    int ds_offset = 3;
    int size = 80 * (height / (720.f));

    // Show the title at the top of the menu
    blueTitle->drawTexture(0, height*(2.f / 3.f), height, height / 3.f, width, height, 1.f, false);

   
    char text[512];
    sprintf(text, "LOADING...");

    xPlacement = (this->width / 2.f) - (size / 2.f) * (strlen(text) / 2.f);
    yPlacement = this->height / 2.f;

    drawDropShadowText(text, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
   
}

void MenuRenderer::drawPause()
{
    int xPlacement;
    int yPlacement;
    int ds_offset = 3;
    int size = 100 * (height / (720.f));

    //Position - insert real position info here
    // X pos calculation is done by dividing the size by 2 (the kerning factor in the function halves it)
    // and then multiplying this by half of the string length


    char gpText[512];
    sprintf(gpText, "GAME PAUSED");

    xPlacement = (width / 2.f) - ((size/2.f) * ((strlen(gpText)+3) / 2.f));
    yPlacement = height / 2.f + 2*size;

    drawDropShadowText(gpText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    
    size = 50 * (height / (720.f));


    //Singleplayer Option
    char spText[512];
    sprintf(spText, "CONTINUE RACE");

    xPlacement = (this->width / 2.f) - (size / 2.f) * ((strlen(spText)+2) / 2.f);
    yPlacement = this->height / 2.f;

    if (selection == 0) {
        drawDropShadowText(spText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(spText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    //Multi-player Option
    char mpText[512];
    sprintf(mpText, "MAIN MENU");

    xPlacement = (this->width / 2.f) - (size / 2.f) * ((strlen(mpText)+2) / 2.f);
    yPlacement = (this->height / 2.f) - size;

    // drawDropShadowText(mpText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    if (selection == 1) {
        drawDropShadowText(mpText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(mpText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
   
    //Quit Option
    char qText[512];
    sprintf(qText, "QUIT GAME");

    xPlacement = (this->width / 2.f) - (size / 2.f) * ((strlen(qText)+2) / 2.f);
    yPlacement = (this->height / 2.f) - (4 * size);

    //drawDropShadowText(qText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    if (selection == 2) {
        drawDropShadowText(qText, whiteText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
    else {
        drawDropShadowText(qText, blueText, blackText, xPlacement, yPlacement, size, ds_offset);
    }
}

void MenuRenderer::drawDropShadowText(const char* string, Text2D* front, Text2D* back, int x, int y, int size, int offset) {
    back->printText2D(string, x, y, size, this->width, this->height);
    front->printText2D(string, x + offset, y - offset, size, this->width, this->height);
}

// This will be likely be used in the window resize callback
void MenuRenderer::setDims(int width, int height) {
    this->width = width;
    this->height = height;
}

void MenuRenderer::registerController(Input * newCont)
{
    controller = newCont;
}

void MenuRenderer::updateMenu()
{
    int numberOfItems = getNumberOfItems(page);
    if (!(controller->LStick_InDeadzone())) {
        if (pressed == false) {
            if(page == MULTI){
                if (controller->LeftStick_X() < -0.98f) {
                    jb->playEffect(Jukebox::menumove);
                    (selection == 0) ? selection += (numberOfItems - 1) : selection--;
                    cout << selection << endl;
                    pressed = true;
                }
                else if (controller->LeftStick_X() > 0.98f) {
                    jb->playEffect(Jukebox::menumove);
                    selection = ((selection + 1) % numberOfItems);
                    cout << selection << endl;
                    pressed = true;
                }
            }
            else {
                if (controller->LeftStick_Y() > 0.98f) {
                    jb->playEffect(Jukebox::menumove);
                    (selection == 0) ? selection += (numberOfItems - 1) : selection--;
                    cout << selection << endl;
                    pressed = true;
                }
                else if (controller->LeftStick_Y() < -0.98f) {
                    jb->playEffect(Jukebox::menumove);
                    selection = ((selection + 1) % numberOfItems);
                    cout << selection << endl;
                    pressed = true;
                }
            }
        }
    }
    else {
        pressed = false;
    }

    switch (page) {
    case MAIN:
        if (controller->GetButtonDown(XButtonIDs::A)) {
            if (aPressed == false) {

                jb->playEffect(Jukebox::menuselect);
                switch (selection) {
                case 0:
                    // page = SINGLE;
                    page = LOADING;
                    numOfPlayers = 1;
                    playing = true;
                    break;
                case 1:
                    page = MULTI;
                    break;
                case 2:
                    selection = 0;
                    page = CREDITS;
                    break;
                case 3:
                    shouldClose = true;
                    break;
                }

                selection = 0;
                aPressed = !aPressed;
            }
        }
        else {
            aPressed = false;
        }

        if (controller->GetButtonDown(XButtonIDs::B)) {

            selection = 0;
            page = MAIN;
        }
        break;
    case CREDITS:
        if (controller->GetButtonDown(XButtonIDs::B)) {
            selection = 0;
            page = MAIN;
        }
        break;
    case PAUSED:
        if (controller->GetButtonDown(XButtonIDs::A)) {
            if (aPressed == false) {

                jb->playEffect(Jukebox::menuselect);
                switch (selection) {
                case 0:
                    // page = SINGLE;
                    //page = LOADING;
                    playing = true;
                    break;
                case 1:
                    page = MAIN;
                    playing = false;
                    break;
                case 2:
                    shouldClose = true;
                    break;
                }

                selection = 0;
                aPressed = !aPressed;
            }
        }
        else {
            aPressed = false;
        }

        if (controller->GetButtonDown(XButtonIDs::B)) {
            playing = true;
        }
        break;
    case MULTI:
        if (controller->GetButtonDown(XButtonIDs::A)) {
            if (aPressed == false) {

                jb->playEffect(Jukebox::menuselect);
                switch (selection) {
                case 0:
                    // page = SINGLE;
                    page = LOADING;
                    playing = true;
                    numOfPlayers = 1;
                    break;
                case 1:
                    page = LOADING;
                    playing = true;
                    numOfPlayers = 2;
                    break;
                case 2:
                    page = LOADING;
                    playing = true;
                    numOfPlayers = 3;
                    break;
                case 3:
                    page = LOADING;
                    numOfPlayers = 4;
                    playing = true;
                    break;
                }

                selection = 0;
                aPressed = !aPressed;
            }
        }
        else {
            aPressed = false;
        }

        if (controller->GetButtonDown(XButtonIDs::B)) {
            selection = 0;
            page = MAIN;
        }
        break;
    }

}

bool MenuRenderer::getPlaying()
{
    return playing;
}

void MenuRenderer::setPlaying(bool play)
{
    playing = play;
}

bool MenuRenderer::getShouldClose()
{
    return shouldClose;
}

void MenuRenderer::setPage(int newPage)
{
    page = newPage;
}

int MenuRenderer::getPage()
{
    return page;
}

int MenuRenderer::getNumberOfItems(int page)
{
    switch (page) {
    case MAIN:
        return 4;
        break;
    case MULTI:
        return 4;
    case PAUSED:
        return 3;
        break;
    default:
        return 1;
        break;
    }
}

int MenuRenderer::getNumOfPlayers()
{
    return numOfPlayers;
}

void MenuRenderer::registerJukebox(Jukebox* jb)
{
    this->jb = jb;
}
