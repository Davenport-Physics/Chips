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

}

int main(int argc, char **argv) 
{

    Init(argc, argv);

    struct timespec current_time, delay_time;
    clock_gettime(CLOCK_REALTIME, &delay_time);

    while (!end_program){

        clock_gettime(CLOCK_REALTIME, &current_time);

        ExecuteNextOpCode();
        EventLoop();
        ControlsLoop();

        if (labs(current_time.tv_nsec - delay_time.tv_nsec) >= 13000000 || current_time.tv_sec - delay_time.tv_sec != 0) {

            DrawScreen();

        }

    }

    QuitDraw();
    QuitSounds();

}

