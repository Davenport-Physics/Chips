#include <stdio.h>

#include "SDL.h"
#include "SDL_events.h"
#include "SDL_keyboard.h"

#include "memory.h"
#include "cpu.h"
#include "draw.h"
#include "shared.h"

SDL_Event event;
static BOOL end_program = FALSE;
static int refresh_rate = 13;

void EventLoop() 
{

    if (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT){
            end_program = TRUE;
        }

    }
    SDL_Delay(refresh_rate);

}

void InitRefreshRate() 
{

    SDL_DisplayMode mode;
    if (SDL_GetDisplayMode(0, 0, &mode) == 0) {

        refresh_rate = (int)((1.0f/((float)mode.refresh_rate)) * 100.0f);
        
    }
    DebugLog("refresh_rate = %dhz\n", mode.refresh_rate);

}

int main(int argc, char **argv) 
{

    InitRefreshRate();
    InitializeMemory(argc, argv);
    InitializeCPU();
    InitDraw();
    DebugTranslateSingleInstruction(0xF033);

    while (!end_program){

        ExecuteNextOpCode();
        EventLoop();
        DrawScreen();

    }

    QuitDraw();

}

