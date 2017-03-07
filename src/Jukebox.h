#pragma once
#include "sdl/include/SDL.h"
#include "sdl/include/SDL_mixer.h"
#include <stdio.h>
#include <string>
#include <vector>

class Jukebox
{
public:
	Mix_Music *clip;
    std::vector<Mix_Chunk*> effects;

	Jukebox();
	~Jukebox();
    void Jukebox::loadMusic(const char* music);
    void Jukebox::loadEffect(const char* effect);
	void Jukebox::play();
    void Jukebox::playEffect(int);
    void Jukebox::setup();

    enum soundEffects
        {
            firehook, gravpull
    };
};

