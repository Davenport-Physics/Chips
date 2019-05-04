#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "shared.h"


static SDL_Window *sdl_window;
static SDL_Renderer *sdl_render;

// Not an efficient use of memory.
static uint_8 display_bits[32][64];
static uint_8 collision_detected = 0;

static int pixel_width_multiplier  = 5;
static int pixel_height_multiplier = 5;

void ClearDrawScreen();

void CreateWindow() 
{

    sdl_window = SDL_CreateWindow("Chips", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
    if (sdl_window == NULL) {

        DebugLog("Could not create window: %s\n", SDL_GetError());
        exit(0);

    }
    sdl_render = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_SOFTWARE);

    ClearDrawScreen();

}

void ResetDisplayBits() 
{

    for (size_t i = 0;i < 32;i++)
        memset(display_bits[i], 0, 64);

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

static SDL_Rect rect;
void RenderRectAtBit(uint_16 x, uint_16 y, uint_8 bit) 
{

    if (bit == 0) {
        SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 255);
    } else {
        SDL_SetRenderDrawColor(sdl_render, 255, 255, 255, 255);
    }

    rect.x = x * pixel_width_multiplier;
    rect.y = y * pixel_height_multiplier;
    rect.w = pixel_width_multiplier;
    rect.h = pixel_height_multiplier;
    if (SDL_RenderFillRect(sdl_render, &rect) !=0 ) {

        DebugLog("Could not render Rect: %s\n", SDL_GetError());
        exit(0);

    }

}

uint_8 CollisionDetected()
{

    return collision_detected;

}

void SetBit(uint_8 y, uint_8 x, uint_8 bit) 
{

    if (y >= 32 || x >= 64) {

        DebugLog("Attempt to conduct an out of bounds access.\n");
        DebugLog("x = %d, y = %d\n", x, y);
        return;

    }

    uint_8 was_set     = display_bits[y][x];
    display_bits[y][x] = display_bits[y][x] ^ bit;
    DebugLog("bit = %d, display_bits = %d, x = %d, y = %d\n", bit, display_bits[y][x], x, y);
    if (was_set && was_set != display_bits[y][x]) {

        DebugLog("Collision at x = %d, y = %d\n", x, y);
        collision_detected = 1;

    }

}

void SetPixels(uint_8 x, uint_8 y, uint_8* bits_to_draw, uint_8 height) 
{

    collision_detected = 0;
    for (size_t i = 0; i < height;i++) {

        int_8 temp = bits_to_draw[i];
        for (size_t j = 0; j < 8; j++) {

            SetBit(y + i, x + j, ((temp & 0x80) >> 7));
            temp = temp << 1;

        }
        
    }
    
}

void ClearDrawScreen() 
{

    SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 255);
    SDL_RenderClear(sdl_render);
    ResetDisplayBits();

}

void RenderCurrentDisplayBits() 
{

    for (size_t i = 0; i < 32;i++) {

        for (size_t j = 0;j < 64;j++) {

            RenderRectAtBit(j, i, display_bits[i][j]);

        }

    }

}

void SetMultipliers() 
{

    SDL_GetWindowSize(sdl_window, &pixel_width_multiplier, &pixel_height_multiplier);
    pixel_width_multiplier  = pixel_width_multiplier  >> 6;
    pixel_height_multiplier = pixel_height_multiplier >> 5;

}

void DrawScreen() 
{

    SDL_SetRenderDrawColor(sdl_render, 0, 0, 0, 255);
    SDL_RenderClear(sdl_render);

    SetMultipliers();

    RenderCurrentDisplayBits();
    SDL_RenderPresent(sdl_render);

}