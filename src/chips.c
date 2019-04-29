#include <stdio.h>

#include "SDL.h"

#include "memory.h"
#include "cpu.h"
#include "draw.h"

int main(int argc, char **argv) 
{

    InitializeMemory(argc, argv);
    InitializeCPU();
    InitDraw();
    DebugTranslateSingleInstruction(0xF033);
    QuitDraw();

}

