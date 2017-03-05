#include "Jukebox.h"

Jukebox::Jukebox(const char* sound)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL failed to initialize");
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("Mixer failed to initialize");
	}

	clip = Mix_LoadMUS(sound);
}

Jukebox::~Jukebox()
{
	Mix_FreeMusic(clip);
	clip = NULL;
	Mix_Quit();
	SDL_Quit();
}

void Jukebox::play()
{
	Mix_PlayMusic(clip, -1);
}
