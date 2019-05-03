#include <stdio.h>
#include <time.h>
#include <math.h>

#include "SDL.h"
#include "SDL_events.h"
#include "SDL_keyboard.h"

#include "sound.h"
#include "controls.h"
#include "memory.h"
#include "cpu.h"
#include "draw.h"
#include "shared.h"

SDL_Event event;
static BOOL end_program = FALSE;
static int refresh_rate = 1;
static struct timespec draw_time;
static struct timespec opcode_time;

void EventLoop() 
{

    if (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT){
            end_program = TRUE;
        }

    }

}

void Init(int argc, char **argv) 
{

    InitializeMemory(argc, argv);
    InitializeCPU();
    InitDraw();
    InitSounds();
    clock_gettime(CLOCK_REALTIME, &draw_time);
    clock_gettime(CLOCK_REALTIME, &opcode_time);

}

void HandleDrawScreenAndInput() 
{

    
    if (diff_time_with_now_in_mill(draw_time) >= 16) {

        DrawScreen();
        clock_gettime(CLOCK_REALTIME, &draw_time);

    }

}

void HandleOpcodeTimer() 
{

    if (diff_time_with_now_in_mill(opcode_time) >= 4) {

        ExecuteNextOpCode();
        EventLoop();
        clock_gettime(CLOCK_REALTIME, &opcode_time);

    }

}

int main(int argc, char **argv) 
{

    Init(argc, argv);
    while (!end_program){

        ControlsLoop();
        HandleOpcodeTimer();
        HandleDrawScreenAndInput();

    }

    QuitDraw();
    QuitSounds();

}

