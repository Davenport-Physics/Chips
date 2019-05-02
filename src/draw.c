#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "shared.h"


static SDL_Window *sdl_window;
static SDL_Renderer *sdl_render;

static SDL_Rect rects[256];
static uint_16 num_rects = 0;

void ClearDrawScreen();

void CreateWindow() 
{

    sdl_window = SDL_CreateWindow("Chips", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_ALLOW_HIGHDPI);
    if (sdl_window == NULL) {

        DebugLog("Could not create window: %s\n", SDL_GetError());
        exit(0);

    }
    sdl_render = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_SOFTWARE);
    ClearDrawScreen();

}

void InitDraw() 
{

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {

        DebugLog("Unable to initialize SDL: %s", SDL_GetError());
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

    rects[num_rects].x = x;
    rects[num_rects].y = y;
    rects[num_rects].w = 4;
    rects[num_rects].h = 5;
    if (SDL_RenderFillRect(sdl_render, &rects[num_rects]) !=0 ) {

        DebugLog("Could not render Rect: %s\n", SDL_GetError());
        exit(0);

    }
    num_rects++;
}

BOOL CollisionDetected()
{

    // TODO optimize this later if possible.
    for (size_t i = 0; i < num_rects-1; i++) {

        for (size_t j = i+1; j < num_rects; j++) {

            if (SDL_HasIntersection(&rects[i], &rects[j]) == SDL_TRUE) {

                return TRUE;

            }

        }

    }
    return FALSE;

}

void DrawPixels(uint_8 x, uint_8 y, uint_8* bits_to_draw, uint_8 height) 
{

    x *= 5;
    y *= 5;
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
    num_rects = 0;

}

void DrawScreen() 
{

    SDL_RenderPresent(sdl_render);

}