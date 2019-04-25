#include <stdio.h>
#include <string.h>

#include "memory.h"

static char v_regs[16];
static unsigned short current_opcode = 0;

void StubOpcode();
void SetupNextTranslation(unsigned short opcode);

struct opcode {
    unsigned short opcode;
    void (*func)();
};

struct opcode opcodes[] = 
{
    {0x0000, &StubOpcode},
    {0x00E0, &StubOpcode}
    {0x00EE, &StubOpcode},
    {0x1000, &StubOpcode},
    {0x2000, &StubOpcode},
    {0x3000, &StubOpcode},
    {0x4000, &StubOpcode},
    {0x5000, &StubOpcode},
    {0x6000, &StubOpcode},
    {0x7000, &StubOpcode},
    {0x8000, &StubOpcode},
    {0x8001, &StubOpcode},
    {0x8002, &StubOpcode},
    {0x8003, &StubOpcode},
    {0x8004, &StubOpcode},
    {0x8005, &StubOpcode},
    {0x8006, &StubOpcode},
    {0x8007, &StubOpcode},
    {0x800E, &StubOpcode},
    {0x9000, &StubOpcode},
    {0xA000, &StubOpcode},
    {0xB000, &StubOpcode},
    {0xC000, &StubOpcode},
    {0xD000, &StubOpcode},
    {0xE09E, &StubOpcode},
    {0xE0A1, &StubOpcode},
    {0xF007, &StubOpcode},
    {0xF00A, &StubOpcode},
    {0xF015, &StubOpcode},
    {0xF018, &StubOpcode},
    {0xF01E, &StubOpcode},
    {0xF029, &StubOpcode},
    {0xF033, &StubOpcode},
    {0xF055, &StubOpcode},
    {0xF065, &StubOpcode}
};

void InitializeCPU() 
{

    memset(v_regs, 0, 16);

}

void ExecuteNextOpCode()
{
    SetupNextTranslation(GetNextOpCode());
}

void SetupNextTranslation(unsigned short opcode)
{

}

void StubOpcode() 
{

}