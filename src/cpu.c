#include <stdio.h>
#include <string.h>

#include "memory.h"

// from left to right.
static const unsigned short FIRST  = 1;
static const unsigned short SECOND = 2;
static const unsigned short THIRD  = 4;
static const unsigned short FOURTH = 8;

static const unsigned short ALL_NIBS = FIRST | SECOND | THIRD | FOURTH;
static const unsigned short FF_NIBS  = FIRST | FOURTH;
static const unsigned short FTF_NIBS = FIRST | THIRD | FOURTH;

static char v_regs[16];
static unsigned short current_opcode = 0;

typedef enum BOOL {

    FALSE = 0,
    TRUE

} BOOL;

BOOL StubOpcode(unsigned short , size_t);
BOOL First(unsigned short opcode, size_t opcodes_idx);
BOOL ExactOpcode(unsigned short opcode, size_t opcodes_idx);
BOOL FirstFourth(unsigned short opcode, size_t opcodes_idx);
BOOL FirstThirdFourth(unsigned short opcode, size_t opcodes_idx);

void SetupNextTranslation(unsigned short);

struct opcode {
    unsigned short opcode;
    BOOL (*translate)(unsigned short, size_t);
    unsigned short necessary_nibbles;
};

static const struct opcode opcodes[35] = 
{
    {0x00E0, &StubOpcode, ALL_NIBS}, // 00E0 disp_clear Clears the screen
    {0x00EE, &StubOpcode, ALL_NIBS}, // 00EE returns from a subroutine
    {0x0000, &StubOpcode, FIRST}, // 0NNN calls program at NN
    {0x1000, &StubOpcode, FIRST}, // 1NNN jumps to address NNN
    {0x2000, &StubOpcode, FIRST}, // 2NNN calls subroutine at NNN
    {0x3000, &StubOpcode, FIRST}, // 3XNN skips the next instruction if Vx == NN
    {0x4000, &StubOpcode, FIRST}, // 4XNN skips the next instruction if Vx != NN
    {0x5000, &StubOpcode, FF_NIBS}, // 5XY0 skips next instruction if VX equals VY
    {0x6000, &StubOpcode, FIRST}, // 6XNN sets VX to NN
    {0x7000, &StubOpcode, FIRST}, // 7XNN add NN to VX (carry flag not changed)
    {0x8000, &StubOpcode, FF_NIBS}, // 8XY0 Sets VX to value of VY
    {0x8001, &StubOpcode, FF_NIBS}, // 8XY1 Sets VX to (VX or VY) (Bitwise OR operation)
    {0x8002, &StubOpcode, FF_NIBS}, // 8XY2 Sets VX to (VX and VY) (Bitwise AND operation)
    {0x8003, &StubOpcode, FF_NIBS}, // 8XY3 Sets VX to (VX xor VY)
    {0x8004, &StubOpcode, FF_NIBS}, // 8XY4 Adds VY to VX. VF is set to 1 when there's a carry, and to 0 otherwise.
    {0x8005, &StubOpcode, FF_NIBS}, // 8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
    {0x8006, &StubOpcode, FF_NIBS}, // 8XY6 Stores the least signification bit of VX in VF and then shifts VX to right by 1
    {0x8007, &StubOpcode, FF_NIBS}, // 8XY7 Sets VX to (VY - VX). VF is set to 0 when there's a borrow, 1 otherwise
    {0x800E, &StubOpcode, FF_NIBS}, // 8XYE Stores the most significant bit of VX in VF and then shifts vx to the left by 1
    {0x9000, &StubOpcode, FF_NIBS}, // 9XY0 Skips the next instruction if VX != VY
    {0xA000, &StubOpcode, FIRST}, // ANNN Sets I to the address of NN
    {0xB000, &StubOpcode, FIRST}, // BNNN Jumps to the address [NNN + V0]
    {0xC000, &StubOpcode, FIRST}, // CXNN Sets VX to the result of a bitwise AND operation on a random number 0-255 and NN
    {0xD000, &StubOpcode, FIRST}, // DXYN Draws a sprite at (VX, VY) with width of 8 and N height in N pixels.
    {0xE09E, &StubOpcode, FTF_NIBS}, // EX9E Skips the instruction if key stored in VX is pressed
    {0xE0A1, &StubOpcode, FTF_NIBS}, // EXA1 Skips next instruction ifkey stores in VX isn't pressed
    {0xF007, &StubOpcode, FTF_NIBS}, // FX07 Sets VX to the value of the delay timer
    {0xF00A, &StubOpcode, FTF_NIBS}, // FX0A A key press is awaited and then stored in VX (All operations blocked until next key event)
    {0xF015, &StubOpcode, FTF_NIBS}, // FX15 Sets the daly timer to VX
    {0xF018, &StubOpcode, FTF_NIBS}, // FX18 Sets the sound timer to VX
    {0xF01E, &StubOpcode, FTF_NIBS}, // FX1E adds VX to I
    {0xF029, &StubOpcode, FTF_NIBS}, // FX29 sets I to the location of the sprite for the character in VX. (Chars 0-F are represented by 4x5 font)
    {0xF033, &StubOpcode, FTF_NIBS}, // FX33 Stores the binary-coded decimal represenation of VX
    {0xF055, &StubOpcode, FTF_NIBS}, // FX55 Stores V0 to VX in memory starting at address I
    {0xF065, &StubOpcode, FTF_NIBS}  // FX65 Fills V0 to VX with values from memory starting at address I 
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

    BOOL translation_success = FALSE;
    for (int i = 0; i < 35; i++) {

        switch(opcodes[i].necessary_nibbles) {
        case FIRST:
            First(opcode, i);
        break;
        case ALL_NIBS:
            ExactOpcode(opcode, i);
        break;
        case FF_NIBS:
            FirstFourth(opcode, i);
        break;
        case FTF_NIBS:
            FirstThirdFourth(opcode, i);
        break;
        }

    }

}

BOOL First(unsigned short opcode, size_t opcodes_idx) 
{
    BOOL translated_opcode = FALSE;

    if ((opcode >> 6) == (opcodes[opcodes_idx].opcode >> 6)) {

        opcodes[opcodes_idx].translate(opcode, opcodes_idx);
        translated_opcode = TRUE;

    }

    return translated_opcode;
}

BOOL ExactOpcode(unsigned short opcode, size_t opcodes_idx) 
{
    BOOL translated_opcode = FALSE;

    if ((opcode ^ opcodes[opcodes_idx].opcode) == 0) {

        opcodes[opcodes_idx].translate(opcode, opcodes_idx);
        translated_opcode = TRUE;

    }

    return translated_opcode;
}

BOOL FirstFourth(unsigned short opcode, size_t opcodes_idx) 
{
    BOOL translated_opcode = FALSE;

    if ((opcode >> 6) == (opcodes[opcodes_idx].opcode >> 6)) {

        if ((opcode << 6) == (opcodes[opcodes_idx].opcode << 6)) {

            opcodes[opcodes_idx].translate(opcode, opcodes_idx);
            translated_opcode = TRUE;

        }

    }

    return translated_opcode;
}

BOOL FirstThirdFourth(unsigned short opcode, size_t opcodes_idx)
{
    BOOL translated_opcode = FALSE;

    int first  = (opcode >> 6) == (opcodes[opcodes_idx].opcode >> 6);
    int third  = ((opcode >> 2) << 3) == ((opcodes[opcodes_idx].opcode >> 2) << 3);
    int fourth = (opcode << 6) == (opcodes[opcodes_idx].opcode << 6);

    if (first && third && fourth) {

        opcodes[opcodes_idx].translate(opcode, opcodes_idx);
        translated_opcode = TRUE;

    }

    return translated_opcode;
}

void DebugTranslateSingleInstruction(unsigned short opcode) 
{

    SetupNextTranslation(opcode);

}

BOOL StubOpcode(unsigned short opcode, size_t opcodes_idx) 
{
    BOOL translated_opcode = FALSE;
    printf("Reached stub function with opcode %04x\n", opcode);
    printf("Compared against opcode %04x\n", opcodes[opcodes_idx].opcode);
    return translated_opcode;

}