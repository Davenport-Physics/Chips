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

void EventLoop() 
{

    if (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT){
            end_program = TRUE;
        }

    }
    SDL_Delay(13);

}

int main(int argc, char **argv) 
{

    InitializeMemory(argc, argv);
    InitializeCPU();
    InitDraw();
    DebugTranslateSingleInstruction(0xF033);

    while (!end_program){
        ExecuteNextOpCode();
        EventLoop();
    }

    QuitDraw();

}

