#include <stdio.h>

#include "SDL_mixer.h"
#include "shared.h"

static const int audio_rate = 22050;
static const int audio_channels = 2;
static const int audio_buffers = 4096;
static const Uint16 audio_format = AUDIO_S16SYS;

static Mix_Music *music = NULL;

void InitSounds() 
{

    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) != 0) {
        
        DebugLog("Unable to initialize audio: %s\n", Mix_GetError());
        exit(0);
        
    }
    music = Mix_LoadMUS("sounds/beep.wav");
    if (music == NULL) {

        DebugLog("Unable to load sounds/beep.wav\n");

    }

}

void QuitSounds() 
{

    Mix_FreeMusic(music);
    Mix_CloseAudio();

}

void PlayBeep() 
{

    if (Mix_PlayMusic(music, 1) == -1) {
        
        DebugLog("Unable to play Ogg file: %s\n", Mix_GetError());
         
    }

}