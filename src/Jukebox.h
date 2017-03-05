#pragma once
#include "sdl/include/SDL.h"
#include "sdl/include/SDL_mixer.h"
#include <stdio.h>
#include <string>

class Jukebox
{
public:
	Mix_Music *clip;

	Jukebox(const char* sound);
	~Jukebox();
	void Jukebox::play();
};

