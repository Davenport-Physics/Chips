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

struct opcode opcodes[] = {
    {0x0000, &StubOpcode}
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