#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "shared.h"


SDL_Window *sdl_window;
SDL_Renderer *sdl_render;

void ClearDrawScreen();

void CreateWindow() 
{

    sdl_window = SDL_CreateWindow("Chips", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_ALLOW_HIGHDPI);
    if (sdl_window == NULL) {

        SDL_Log("Could not create window: %s\n", SDL_GetError());
        exit(0);

    }
    sdl_render = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_SOFTWARE);
    ClearDrawScreen();

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

void RenderRectAtBitIfNecessary(uint_8 x, uint_8 y, uint_8 bit) 
{

    if (bit == 0) {
        return;
    }

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = 4;
    rect.h = 5;
    if (SDL_RenderFillRect(sdl_render, &rect) !=0 ) {

        SDL_Log("Could not render Rect: %s\n", SDL_GetError());

    }
}

void DrawPixels(uint_8 x, uint_8 y, uint_8* bits_to_draw, uint_8 height) 
{

    SDL_SetRenderDrawColor(sdl_render, 255, 255, 255, 255);
    for (size_t i = 0; i < height;i++) {

        int_8 temp = bits_to_draw[i];
        for (size_t j = 0; j < 8; j++) {

            temp = temp << j;
            RenderRectAtBitIfNecessary(x+j*4, y+i*5, temp & 0x80);

        }
        
    }
    
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