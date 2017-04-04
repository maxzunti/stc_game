#pragma once
#include "sdl/include/SDL.h"
#include "sdl/include/SDL_mixer.h"
#include <stdio.h>
#include <string>
#include <vector>

class Jukebox
{
    std::vector<Mix_Chunk*> engine_sounds;
    void loadEngineSounds();
    std::string engine_sound_prefix = "assets/sound/mix_engine/car";

public:
    const int max_engine_sounds = 30;
    const int num_engine_sounds = 20;
    const float engine_volume_mul = .1;
	Mix_Music *clip;
    std::vector<Mix_Chunk*> effects;

	Jukebox();
	~Jukebox();
    void Jukebox::loadMusic(const char* music);
    void Jukebox::loadEffect(const char* effect);
	void Jukebox::play();
    void Jukebox::playEffect(int);
    int Jukebox::revEngine(bool idle, int channel);
    void Jukebox::stop(int channel);
    int Jukebox::isPlaying(int channel);
    void Jukebox::setup();
    void Jukebox::stopMusic();
    int Jukebox::honkHorn(int channel);
    int playEngineSound(int speedNum, int channel);

    enum soundEffects
    {
        firehook, gravpull, metalhit, hollowhit, revengine, idle, menumove, menuselect, horn
    };
};

