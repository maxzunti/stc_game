#include "Skyline.h"
#include <iostream>
#include <math.h>


using namespace std;

//void Skyline::genCubes() {

//}
Skyline::Skyline(int mmSize, GLuint &mm_frameBuffer, float scale, std::vector<Renderable*>& ents, Input* cont) :
    sl_parser("config/sl_config2", &slParams)
    {
    controller = cont;

    initParams();
    updateFromConfig();

    for (int i = 0; i < NUM_X; i++) {
        for (int j = 0; j < NUM_Y; j++) {
            cubes[i][j] = NULL;
        }
    }

    if (mmSize != 360) { // default & expected size
        std::cout << "WARNING: expected Skyline minimap size to be 360, got " << mmSize << endl;
        // This is janky, but we choose to use 360 for consistency
        mmSize = 360;
    }
    vector<vector<int>> mapDrawn;

    GLfloat* pix = (GLfloat*)malloc(sizeof(GLfloat) * 4);
    if (pix) {

        glBindFramebuffer(GL_FRAMEBUFFER, mm_frameBuffer);
        //      glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glViewport(0, 0, mmSize, mmSize);
        glViewport(0, 0, mmSize, mmSize);
        int i = 0;
        int j = 0;

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        int nz = 0;
        int z = 0;
        while (j < mmSize) {
            vector<int> yVals;
            while (i < mmSize) {
                glReadPixels(i, j, 1, 1, GL_RGBA, GL_FLOAT, pix);
                GLfloat pixel = *pix;
                if (pixel == 0) {
                    z++;
                    yVals.push_back(1);
                }
                else {
                    yVals.push_back(0);
                    nz++;
                }
                i += Y_PIXEL_SPACING;
            }
            mapDrawn.push_back(yVals);
            j += X_PIXEL_SPACING;
            i = 0;
        }

        // Now, use mapDrawn to fill in a larger grid of vectors
        vector<vector<int>> full = fillGrid(mapDrawn);
        drawGrid(full);

        makeBuildings(full, scale, ents);
    }
    else {
        cout << "Error: unable to allocate room for Skyline gen" << endl;
    }
    CheckGLErrors("skyline init");
    free(pix);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    updateFromConfig();
}

void Skyline::initParams() {
  //  slParams.push_back(std::make_pair(std::string("HOOK_SPEED"), &HOOK_SPEED));
  // pixel widths, used for sampling the minimap texture
    slParams.push_back(std::make_pair(std::string("X_SCL"), &X_SCL));
    slParams.push_back(std::make_pair(std::string("Y_SCL"), &Y_SCL));
    slParams.push_back(std::make_pair(std::string("Z_SCL"), &Z_SCL));
    slParams.push_back(std::make_pair(std::string("LOCAL_ROT"), &LOCAL_ROT));
    slParams.push_back(std::make_pair(std::string("TOTAL_ROT"), &TOTAL_ROT));

    slParams.push_back(std::make_pair(std::string("Y_OFFSET"), &Y_OFFSET));
    slParams.push_back(std::make_pair(std::string("TOTAL_SCALE"), &TOTAL_SCALE));
    slParams.push_back(std::make_pair(std::string("CENTER_X"), &CENTER_X));
    slParams.push_back(std::make_pair(std::string("CENTER_Y"), &CENTER_Y));
    slParams.push_back(std::make_pair(std::string("CENTER_Z"), &CENTER_Z));

    slParams.push_back(std::make_pair(std::string("X_FREQ"), &X_FREQ));
    slParams.push_back(std::make_pair(std::string("Y_FREQ"), &Y_FREQ));
    slParams.push_back(std::make_pair(std::string("X_AMP"), &X_AMP));
    slParams.push_back(std::make_pair(std::string("Y_AMP"), &Y_AMP));
    slParams.push_back(std::make_pair(std::string("X_PERIODS"), &X_PERIODS));
    slParams.push_back(std::make_pair(std::string("Y_PERIODS"), &Y_PERIODS));

    slParams.push_back(std::make_pair(std::string("X_PHASE"), &X_PHASE));
    slParams.push_back(std::make_pair(std::string("Y_PHASE"), &Y_PHASE));
    slParams.push_back(std::make_pair(std::string("X_PHASE_STEP"), &X_PHASE_STEP));
    slParams.push_back(std::make_pair(std::string("Y_PHASE_STEP"), &Y_PHASE_STEP));

    slParams.push_back(std::make_pair(std::string("CUBE_SPACING"), &CUBE_SPACING));
}

Skyline::~Skyline() {
    // deletion happens to all entities in gameState
    for (int i = 0; i < NUM_X; i++) {
        for (int j = 0; j < NUM_Y; j++) {
            if (cubes[i][j] != NULL) {
                delete cubes[i][j];
            }
        }
    }
}


void Skyline::makeBuildings(std::vector<std::vector<int>> grid, float scale, std::vector<Renderable*>& ents) {
    if (NUM_X != grid.size()) {
        cout << "WARNING: expected grid-with of " << NUM_X << ", got " << grid.size();
    }
    if (NUM_Y != grid.at(0).size()) {
        cout << "WARNING: expected grid-height of " << NUM_X << ", got " << grid.at(0).size();
    }
    Renderable * init = new Renderable("assets/models/Crate/Crate1.obj", "assets/textures/lg.png");
    init->scale(X_SCL, Y_SCL, Z_SCL);
    init->getModels().at(0)->tile_UV_Y(Y_SCL / X_SCL);
    init->SIL_Y_SCALE = 1.01;
    init->scaleModels();
    Renderable * init2 = new Renderable("assets/models/Crate/Crate1.obj", "assets/textures/dg.png");
    init2->scale(X_SCL, Y_SCL, Z_SCL);
    init2->getModels().at(0)->tile_UV_Y(Y_SCL / X_SCL);
    init2->SIL_Y_SCALE = 1.01;
    init2->scaleModels();
    Renderable * init3 = new Renderable("assets/models/Crate/Crate1.obj", "assets/textures/purp.png");
    init3->scale(X_SCL, Y_SCL, Z_SCL);
    init3->getModels().at(0)->tile_UV_Y(Y_SCL / X_SCL);
    init3->SIL_Y_SCALE = 1.01;
    init3->scaleModels();


    for (int i = 0; i < grid.size() - 1; i++) {
        for (int j = 0; j < grid.at(0).size() - 1; j++) {
            // Look ahead for valid building placement
            if ((grid[i][j] == 1) &&
                (grid[i + 1][j] == 1) &&
                (grid[i][j + 1] == 1) &&
                (grid[i + 1][j + 1] == 1)) {
                int dnum = rand() % 100;
                if (dnum < density_prob) {

                    int rnum = rand() % 100;
                    Renderable* building;
                    if (rnum < purp_prob) {
                        building = new Renderable(init3->getModels());
                    }
                    else if (rnum < (dark_prob + purp_prob)) {
                        building = new Renderable(init2->getModels());
                    }
                    else {
                        building = new Renderable(init->getModels());
                    }
                    glm::vec3 newPos = glm::vec3(i * 0.5, 0.0, j * 0.5) * scale * CUBE_SPACING;
                    building->setPos(newPos);
                    ents.push_back(building);
                    cubes[i][j] = building;
                }
            }
        }
    }
}

std::vector<std::vector<int>> Skyline::fillGrid(std::vector < std::vector<int>> inner) {
    vector<vector<int>> fullGrid;
    int i = 0;
    // Fill the outer 'top' of the grid
    while (i < (NUM_X - inner.size()) / 2) {
        vector<int> grid;
        for (int k = 0; k < NUM_Y; k++) {
            grid.push_back(1);
        }
        fullGrid.push_back(grid);
        i++;
    }

    // Middle section: fill the outsides with buildings, but copy the rows from 'inner' to the center
    while (i < ((NUM_X - inner.size()) / 2) + inner.size()) {
        vector<int> line;
        for (int k = 0; k < (NUM_Y - inner.at(0).size()) / 2; k++) {
            line.push_back(1);
        }
        for (int &v : inner.at(i - ((NUM_X - inner.size()) / 2))) {
            line.push_back(v);
        }
        for (int k = 0; k < (NUM_Y - inner.at(0).size()) / 2; k++) {
            line.push_back(1);
        }
        fullGrid.push_back(line);
        i++;
    }

    // Filled the center; now fill the bottom
    while (i < NUM_X) {
        vector<int> grid;
        for (int k = 0; k < NUM_Y; k++) {
            grid.push_back(1);
        }
        fullGrid.push_back(grid);
        i++;
    }
    return fullGrid;
}

void Skyline::drawGrid(std::vector<std::vector<int>>& grid) {
    cout << "*********** Drawing Grid **************" << endl;
    for (auto& outer : grid) {
        for (int& inner : outer) {
            cout << " " << inner;
        }
        cout << endl;
    }
}

void Skyline::update() {
    frameCounter++;

    X_PHASE += X_PHASE_STEP;
    Y_PHASE += Y_PHASE_STEP;

    float x_blocks_per_spatial_period = NUM_X / X_PERIODS;
    float y_blocks_per_spatial_period = NUM_Y / Y_PERIODS;
    // Drive sin/cosine waves
    for (int i = 0; i < NUM_X; i++) {
        float x_angle = X_PHASE + ((2 * M_PI) * (i / x_blocks_per_spatial_period)) + (((2 * M_PI) / (60. * X_FREQ)) * frameCounter);
        float x_offset = X_AMP * sin(x_angle);
        for (int j = 0; j < NUM_Y; j++) {
            if (cubes[i][j]) {
                float y_angle = Y_PHASE + ((2 * M_PI) * (j / y_blocks_per_spatial_period)) + (((2 * M_PI) / (60. * Y_FREQ)) * frameCounter);
                float y_offset = Y_AMP * sin(y_angle);
                cubes[i][j]->setPos(cubes[i][j]->xPos(), Y_OFFSET + x_offset + y_offset, cubes[i][j]->zPos());
            }
        }
    }

    if (controller->GetButtonPressed(XButtonIDs::X)) {
        updateFromConfig();
    }
}

void Skyline::updateFromConfig() {
    sl_parser.updateFromFile();
    
    for (int i = 0; i < NUM_X; i++) {
        for (int j = 0; j < NUM_Y; j++) {
            if (cubes[i][j] != NULL) {
                glm::vec3 newPos = glm::vec3(i * 0.5, 0.0, j * 0.5) * TOTAL_SCALE * CUBE_SPACING;
                newPos.x += CENTER_X;
                newPos.y += CENTER_Y;
                newPos.z += CENTER_Z;
                cubes[i][j]->setPos(newPos);

                cubes[i][j]->reset_scale();
                cubes[i][j]->scale(X_SCL, Y_SCL, Z_SCL);
            }
        }
    }
}