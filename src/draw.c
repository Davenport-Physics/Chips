#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "shared.h"


SDL_Window *sdl_window;
SDL_Renderer *sdl_render;

void CreateWindow() 
{

    sdl_window = SDL_CreateWindow("Chips", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    if (sdl_window == NULL) {

        SDL_Log("Could not create window: %s\n", SDL_GetError());
        exit(0);

    }
    sdl_render = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_SOFTWARE);

}

void InitDraw() 
{

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {

        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(0);

    }
    CreateWindow();

}

void QuitDraw() 
{

    SDL_DestroyWindow(sdl_window);
    SDL_Quit();

}

void DrawPixels(int_8 x, int_8 y, int_8* bits_to_draw, int_8 height) 
{

    // TODO translate bits_to_draw
    SDL_Rect rect;
    SDL_RenderDrawRect(sdl_render, &rect);
}

void ClearDrawScreen() 
{

    SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 255);
    SDL_RenderClear(sdl_render);


}

void DrawScreen() 
{

    SDL_RenderPresent(sdl_render);


}