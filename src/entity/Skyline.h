#pragma once
#include "Entity.h"
#include "Renderable.h"
#include "glm\vec3.hpp"
#include "../input/input.h"
#include "../util/ConfigParser.h"

class Skyline : public Entity {
    Input* controller;
    ConfigParser sl_parser;

    fp_vars slParams;

    int frameCounter = 0; // update once per frame
    static const int NUM_X = 60; // number of buildings in the x-dir map 1 is 26
    static const int NUM_Y = 60; // '' y-dir map 1 is 26
    // use even no's

    // pixel widths, used for sampling the minimap texture
    float X_PIXEL_SPACING = 6; // map 1 is 20
    float Y_PIXEL_SPACING = 6; // map 1 is 20

    // proportions of each 'block' to occupy (could randomize, but let's try this)
    float X_SCL = 20.0;
    float Y_SCL = 100.0;
    float Z_SCL = 20.0;
    float LOCAL_ROT = 0.0;
    float TOTAL_ROT = 0.0;
    
    float Y_OFFSET = -50;
    float TOTAL_SCALE = 50;
    
    // useful for square pattern; useless for radial pulse
    float X_FREQ = 4.;
    float Y_FREQ = 2.;

    float X_AMP = 50;
    float Y_AMP = 50;

    float X_PERIODS = 2.0;
    float Y_PERIODS = 2.0;
    float X_PHASE = 0.0;
    float Y_PHASE = 0.0;
    float X_PHASE_STEP = 0.0;
    float Y_PHASE_STEP = 0.0;

    float CENTER_X = 0;
    float CENTER_Y = 0;
    float CENTER_Z = 0;

    float CUBE_SPACING = 3.0f;

    Renderable* cubes[NUM_X][NUM_Y];
    std::vector<std::vector<int>> fillGrid(std::vector<std::vector<int>> inner);
    void makeBuildings(std::vector<std::vector<int>>, float scale, std::vector<Renderable*>& ents);
    void drawGrid(std::vector<std::vector<int>>& grid); // debug only
    void updateFromConfig();
    void initParams();

    int dark_prob = 30;
    int purp_prob = 10;
    int density_prob = 20; // map 1 is 30

public:
    Skyline(int mmSize, GLuint &mm_frameBuffer, float scale, std::vector<Renderable*>& ents, Input* cont = NULL);
    ~Skyline();
    void update();
};