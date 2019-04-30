#include <stdio.h>

#include "SDL.h"
#include "SDL_events.h"
#include "SDL_keyboard.h"

#include "shared.h"

SDL_Event event;

typedef struct control_conversion {

    uint_8 chip_8_control;
    SDL_Scancode code;

} controls;

static const controls convert[16] = 
{
    {0x0, SDL_SCANCODE_1},
    {0x1, SDL_SCANCODE_2},
    {0x2, SDL_SCANCODE_D},
    {0x3, SDL_SCANCODE_3},
    {0x4, SDL_SCANCODE_S},
    {0x5, SDL_SCANCODE_4},
    {0x6, SDL_SCANCODE_A},
    {0x7, SDL_SCANCODE_Q},
    {0x8, SDL_SCANCODE_W},
    {0x9, SDL_SCANCODE_E},
    {0xA, SDL_SCANCODE_R},
    {0xB, SDL_SCANCODE_F},
    {0xC, SDL_SCANCODE_Z},
    {0xD, SDL_SCANCODE_X},
    {0xE, SDL_SCANCODE_C},
    {0xF, SDL_SCANCODE_V}
};


SDL_Scancode PollForScancode() 
{

    if (SDL_PollEvent(&event) && event.type == SDL_KEYDOWN) {

        return event.key.keysym.scancode;

    }
    return SDL_SCANCODE_AUDIONEXT;

}

uint_8 GetTranslationOfScancodeIfAny(SDL_Scancode code) 
{

    for (size_t i = 0; i < 16; i++) {

        if (code == convert[i].code) {

            return convert[i].chip_8_control;

        }

    }

    return 0x11;

} 

BOOL IsKeyPressed(uint_16 control) 
{

    SDL_Scancode code = PollForScancode();
    if (code != SDL_SCANCODE_AUDIONEXT) {

        return convert[control].code == code;

    }
    return FALSE;

}

uint_8 AwaitKeyPress() 
{
    BOOL found_suitable_key = FALSE;
    uint_8 control;
    while (!found_suitable_key) {

        control = GetTranslationOfScancodeIfAny(PollForScancode());
        
        if (control != 0x11) {

            found_suitable_key = TRUE;

        }

    }
    return control;

}

