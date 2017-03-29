#pragma once
#include "Entity.h"
#include "Renderable.h"
#include "../renderer/model/Texture.h";
class Skyline : public Entity {
    static const int NUM_X = 40; // number of buildings in the x-dir
    static const int NUM_Y = 40; // '' y-dir

    // pixel widths, used for sampling the minimpa texture
    float X_PIXEL_SPACING = 20;
    float Y_PIXEL_SPACING = 20;

    // proportions of each 'block' to occupy (could randomize, but let's try this)
    float X_SCL = 0.9;
    float Y_SCL = 0.9;
    float HEIGHT = 50;
    
    float Y_OFFSET = 0;
    
    // useful for square pattern; useless for radial pulse
    float X_FREQ = 0.5;
    float Y_FREQ = 1.;

    float RAD_FREQ = 1.0;
    float AMPLITUDE = 50;

    Renderable* cubes[NUM_X][NUM_Y];

public:
    Skyline(int mmSize, GLuint &mm_frameBuffer, const std::vector<Entity*>& ents);


};