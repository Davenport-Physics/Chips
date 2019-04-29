#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>


SDL_Window *sdl_window;

void CreateWindow() 
{

    sdl_window = SDL_CreateWindow("Chips", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 64, 32, SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    if (sdl_window == NULL) {

        SDL_Log("Could not create window: %s\n", SDL_GetError());

    }

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

void ClearDrawScreen() 
{



}

void DrawPixels() 
{



}