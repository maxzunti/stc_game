#include "Jukebox.h"
#include <iostream>

Jukebox::Jukebox()
{

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL failed to initialize");
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("Mixer failed to initialize");
	}

    Mix_AllocateChannels(256);

}

Jukebox::~Jukebox()
{
	Mix_FreeMusic(clip);
    for (Mix_Chunk* m : this->effects)
    {
        Mix_FreeChunk(m);
    }
	clip = NULL;
	Mix_Quit();
	SDL_Quit();
}

void Jukebox::loadMusic(const char* music)
{
    this->clip = Mix_LoadMUS(music);
}


void Jukebox::loadEffect(const char* effect)
{
    this->effects.push_back(Mix_LoadWAV(effect));
}

void Jukebox::play()
{
	Mix_PlayMusic(clip, -1);
}


void Jukebox::playEffect(int effect)
{
    int playchannel = Mix_PlayChannel(-1, this->effects.at(effect), 0);
}

int Jukebox::revEngine(bool idle)
{
    if (!idle)
        return Mix_PlayChannel(100, this->effects.at(Jukebox::revengine), -1);
    else
        return Mix_PlayChannel(101, this->effects.at(Jukebox::idle), -1);
}


void Jukebox::stop(int channel)
{
    Mix_HaltChannel(channel);
}

int Jukebox::isPlaying(int channel)
{
    return Mix_Playing(channel);
}

void Jukebox::setup()
{
    this->loadMusic("assets/sound/dmw.mp3");
    this->loadEffect("assets/sound/laser.mp3");
    this->loadEffect("assets/sound/pull.mp3");
    this->loadEffect("assets/sound/metal.mp3");
    this->loadEffect("assets/sound/shoot.mp3");
    this->loadEffect("assets/sound/revengine.mp3");
    this->loadEffect("assets/sound/idle.mp3");
}