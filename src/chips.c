#include <stdio.h>

#include "memory.h"
#include "cpu.h"

int main(int argc, char **argv) 
{

    InitializeMemory(argc, argv);
    InitializeCPU();
    DebugTranslateSingleInstruction(0xF033);

}

