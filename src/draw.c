#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "shared.h"


static SDL_Window *sdl_window;
static SDL_Renderer *sdl_render;

// Not an efficient use of memory.
static uint_8 display_bits[64][32];
static uint_8 collision_detected = 0;

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

void RenderRectAtBitIfNecessary(uint_16 x, uint_16 y, uint_8 bit) 
{

    if (bit == 0) {
        return;
    }

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = 5;
    rect.h = 5;
    if (SDL_RenderFillRect(sdl_render, &rect) !=0 ) {

        DebugLog("Could not render Rect: %s\n", SDL_GetError());
        exit(0);

    }

}

uint_8 CollisionDetected()
{

    return collision_detected;

}

void SetBit(uint_8 x, uint_8 y, uint_8 bit) 
{

    if (bit != 0 && display_bits[x][y] != 0) {

        collision_detected = 1;
        display_bits[x][y] = 0;

    } else { 

        display_bits[x][y] = 1;

    }

}

void SetPixels(uint_8 x, uint_8 y, uint_8* bits_to_draw, uint_8 height) 
{

    collision_detected = 0;
    for (size_t i = 0; i < height;i++) {

        int_8 temp = bits_to_draw[i];
        for (size_t j = 0; j < 8; j++) {

            SetBit(x, y, temp & 0x80);
            temp = temp << 1;

        }
        
    }
    
}

void ClearDrawScreen() 
{

    SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 255);
    SDL_RenderClear(sdl_render);

}

void RenderCurrentDisplayBits() 
{

    SDL_SetRenderDrawColor(sdl_render, 255, 255, 255, 255);
    for (size_t i = 0; i < 64;i++) {

        for (size_t j = 0;j < 32;i++) {

            RenderRectAtBitIfNecessary(i*5, j*5, display_bits[i][j]);

        }

    }

}

void DrawScreen() 
{

    ClearDrawScreen();
    RenderCurrentDisplayBits();
    SDL_RenderPresent(sdl_render);

}