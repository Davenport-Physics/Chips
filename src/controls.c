#include <stdio.h>
#include <time.h>

#include "SDL.h"
#include "SDL_events.h"
#include "SDL_keyboard.h"

#include "shared.h"

SDL_Event event;

typedef struct control_conversion {

    uint_8 chip_8_control;
    SDL_Scancode code;

} controls;

typedef struct controls_pressed {

    BOOL was_pressed;
    clock_t timestamp;

} controls_pressed;

static const controls convert[16] = 
{
    {0x0, SDL_SCANCODE_1},
    {0x1, SDL_SCANCODE_2},
    {0x2, SDL_SCANCODE_S},
    {0x3, SDL_SCANCODE_3},
    {0x4, SDL_SCANCODE_A},
    {0x5, SDL_SCANCODE_4},
    {0x6, SDL_SCANCODE_D},
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

static controls_pressed pressed[16];
static unsigned int CLOCKS_PER_MS = CLOCKS_PER_SEC / 1000;

SDL_Scancode PollForScancode() 
{

    if (SDL_PollEvent(&event) && event.type == SDL_KEYDOWN) {

        return event.key.keysym.scancode;

    }
    return SDL_SCANCODE_AUDIONEXT;

}

const uint_8 *PollForKeyStates() 
{

    SDL_PumpEvents();
    return SDL_GetKeyboardState(NULL);

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

    if (pressed[control].was_pressed) {

        clock_t current_clock_time = clock();
        if ((current_clock_time - pressed[control].timestamp)/(CLOCKS_PER_MS) <= 15) {

            return TRUE;

        } else {

            pressed[control].was_pressed = FALSE;

        }

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
        SDL_Delay(10);

    }
    return control;

}

void ProcessScanCodeForTimestamp(SDL_Scancode code) 
{

    for (size_t i = 0; i < 16;i++) {

        if (convert[i].code == code) {

            pressed[i].was_pressed = TRUE;
            pressed[i].timestamp   = clock();

        }

    }

}

void ControlsLoop() 
{

    const uint_8 *key_states = PollForKeyStates();
    for (size_t i = 0; i < 16; i++) {

        if (key_states[convert[i].code]) {

            ProcessScanCodeForTimestamp(convert[i].code);

        }

    }

}

