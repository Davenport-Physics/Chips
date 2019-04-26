#include <stdio.h>
#include <string.h>

#include "memory.h"

// from left to right.
static const int FIRST  = 1;
static const int SECOND = 2;
static const int THIRD  = 4;
static const int FOURTH = 8;

static const int ALL_NIBS = FIRST | SECOND | THIRD | FOURTH;
static const int FF_NIBS  = FIRST | FOURTH;
static const int FTF_NIBS = FIRST | THIRD | FOURTH;

static char v_regs[16];
static unsigned short current_opcode = 0;

typedef enum BOOL {

    FALSE = 0,
    TRUE

} BOOL;

BOOL First(unsigned short opcode, size_t opcodes_idx);
BOOL ExactOpcode(unsigned short opcode, size_t opcodes_idx);
BOOL FirstFourth(unsigned short opcode, size_t opcodes_idx);
BOOL FirstThirdFourth(unsigned short opcode, size_t opcodes_idx);

/* Register functions */
void ClearScreen(unsigned short);
void Return(unsigned short);
void CallProg(unsigned short);
void Jump(unsigned short);
void CallSubroutine(unsigned short);

void Skip_IfVx_EqualNN(unsigned short);
void Skip_IfVx_DoesNotEqualNN(unsigned short);
void Skip_IfVx_Equal_Vy(unsigned short);

void Set_Vx_To_NN(unsigned short);
void Add_NN_To_Vx(unsigned short);

void Vx_ToVy(unsigned short);
void Vx_To_VxOrVy(unsigned short);
void Vx_To_VxAndVy(unsigned short);
void Vx_To_VxXorVy(unsigned short);

void Add_Vy_To_VxCarry(unsigned short);
void Sub_Vy_From_VxBorrow(unsigned short);
void Store_Least_Sig_Shift_Right(unsigned short);

void Vx_To_VyMinusVxBorrow(unsigned short);
void Store_Most_Sig_Shift_Left(unsigned short);
void SkipNextIf_VxDoesNotEqual_Vy(unsigned short);
void Set_I_to_NN(unsigned short);
void Jump_ToAdressNNPlusV0(unsigned short);
void SetVx_To_RandomAndNN(unsigned short);
void DrawSrite(unsigned short);
void Skip_Instruction_If_Key_IsPressed(unsigned short);
void Skip_Instruction_If_Key_Not_Pressed(unsigned short);
void SetVXToDelayTimer(unsigned short);
void GetBlockingKeyPress(unsigned short);
void SetDelayTimer(unsigned short);
void SetSoundTimer(unsigned short);
void AddVx_To_I(unsigned short);
void Set_I_ToLocationOfSprite(unsigned short);
void StoreVx(unsigned short);
void StoreAllVs(unsigned short);
void FillAllVs(unsigned short);

void SetupNextTranslation(unsigned short);

struct opcode {
    unsigned short opcode;
    void (*translate)(unsigned short);
    int necessary_nibbles;
};

static const struct opcode opcodes[35] = 
{
    {0x00E0, &ClearScreen                        , ALL_NIBS}, // 00E0 disp_clear Clears the screen
    {0x00EE, &Return                             , ALL_NIBS}, // 00EE returns from a subroutine
    {0x0000, &CallProg                           , FIRST}, // 0NNN calls program at NN
    {0x1000, &Jump                               , FIRST}, // 1NNN jumps to address NNN
    {0x2000, &CallSubroutine                     , FIRST}, // 2NNN calls subroutine at NNN
    {0x3000, &Skip_IfVx_EqualNN                  , FIRST}, // 3XNN skips the next instruction if Vx == NN
    {0x4000, &Skip_IfVx_DoesNotEqualNN           , FIRST}, // 4XNN skips the next instruction if Vx != NN
    {0x5000, &Skip_IfVx_Equal_Vy                 , FF_NIBS}, // 5XY0 skips next instruction if VX equals VY
    {0x6000, &Set_Vx_To_NN                       , FIRST}, // 6XNN sets VX to NN
    {0x7000, &Add_NN_To_Vx                       , FIRST}, // 7XNN add NN to VX (carry flag not changed)
    {0x8000, &Vx_ToVy                            , FF_NIBS}, // 8XY0 Sets VX to value of VY
    {0x8001, &Vx_To_VxOrVy                       , FF_NIBS}, // 8XY1 Sets VX to (VX or VY) (Bitwise OR operation)
    {0x8002, &Vx_To_VxAndVy                      , FF_NIBS}, // 8XY2 Sets VX to (VX and VY) (Bitwise AND operation)
    {0x8003, &Vx_To_VxXorVy                      , FF_NIBS}, // 8XY3 Sets VX to (VX xor VY)
    {0x8004, &Add_Vy_To_VxCarry                  , FF_NIBS}, // 8XY4 Adds VY to VX. VF is set to 1 when there's a carry, and to 0 otherwise.
    {0x8005, &Sub_Vy_From_VxBorrow               , FF_NIBS}, // 8XY5 VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
    {0x8006, &Store_Least_Sig_Shift_Right        , FF_NIBS}, // 8XY6 Stores the least signification bit of VX in VF and then shifts VX to right by 1
    {0x8007, &Vx_To_VyMinusVxBorrow              , FF_NIBS}, // 8XY7 Sets VX to (VY - VX). VF is set to 0 when there's a borrow, 1 otherwise
    {0x800E, &Store_Most_Sig_Shift_Left          , FF_NIBS}, // 8XYE Stores the most significant bit of VX in VF and then shifts vx to the left by 1
    {0x9000, &SkipNextIf_VxDoesNotEqual_Vy       , FF_NIBS}, // 9XY0 Skips the next instruction if VX != VY
    {0xA000, &Set_I_to_NN                        , FIRST}, // ANNN Sets I to the address of NN
    {0xB000, &Jump_ToAdressNNPlusV0              , FIRST}, // BNNN Jumps to the address [NNN + V0]
    {0xC000, &SetVx_To_RandomAndNN               , FIRST}, // CXNN Sets VX to the result of a bitwise AND operation on a random number 0-255 and NN
    {0xD000, &DrawSrite                          , FIRST}, // DXYN Draws a sprite at (VX, VY) with width of 8 and N height in N pixels.
    {0xE09E, &Skip_Instruction_If_Key_IsPressed  , FTF_NIBS}, // EX9E Skips the instruction if key stored in VX is pressed
    {0xE0A1, &Skip_Instruction_If_Key_Not_Pressed, FTF_NIBS}, // EXA1 Skips next instruction ifkey stores in VX isn't pressed
    {0xF007, &SetVXToDelayTimer                  , FTF_NIBS}, // FX07 Sets VX to the value of the delay timer
    {0xF00A, &GetBlockingKeyPress                , FTF_NIBS}, // FX0A A key press is awaited and then stored in VX (All operations blocked until next key event)
    {0xF015, &SetDelayTimer                      , FTF_NIBS}, // FX15 Sets the daly timer to VX
    {0xF018, &SetSoundTimer                      , FTF_NIBS}, // FX18 Sets the sound timer to VX
    {0xF01E, &AddVx_To_I                         , FTF_NIBS}, // FX1E adds VX to I
    {0xF029, &Set_I_ToLocationOfSprite           , FTF_NIBS}, // FX29 sets I to the location of the sprite for the character in VX. (Chars 0-F are represented by 4x5 font)
    {0xF033, &StoreVx                            , FTF_NIBS}, // FX33 Stores the binary-coded decimal represenation of VX
    {0xF055, &StoreAllVs                         , FTF_NIBS}, // FX55 Stores V0 to VX in memory starting at address I
    {0xF065, &FillAllVs                          , FTF_NIBS}  // FX65 Fills V0 to VX with values from memory starting at address I 
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

		if (opcodes[i].necessary_nibbles == FIRST) 
		{

			First(opcode, i);

		} else if (opcodes[i].necessary_nibbles == ALL_NIBS) {

			ExactOpcode(opcode, i);

		} else if (opcodes[i].necessary_nibbles == FF_NIBS) {

			FirstFourth(opcode, i);

		} else if (opcodes[i].necessary_nibbles == FTF_NIBS) {

			FirstThirdFourth(opcode, i);

		}

    }

}

BOOL First(unsigned short opcode, size_t opcodes_idx) 
{
    BOOL translated_opcode = FALSE;

    if ((opcode & 0xF00) == (opcodes[opcodes_idx].opcode & 0xF000)) {

        opcodes[opcodes_idx].translate(opcode);
        translated_opcode = TRUE;

    }

    return translated_opcode;
}

BOOL ExactOpcode(unsigned short opcode, size_t opcodes_idx) 
{
    BOOL translated_opcode = FALSE;

    if ((opcode ^ opcodes[opcodes_idx].opcode) == 0) {

        opcodes[opcodes_idx].translate(opcode);
        translated_opcode = TRUE;

    }

    return translated_opcode;
}

BOOL FirstFourth(unsigned short opcode, size_t opcodes_idx) 
{
    BOOL translated_opcode = FALSE;

	int first  = (opcode & 0xF000) == (opcodes[opcodes_idx].opcode & 0xF000);
	int fourth = (opcode & 0x000F) == (opcodes[opcodes_idx].opcode & 0x000F);
    if (first && fourth) {

		opcodes[opcodes_idx].translate(opcode);
       	translated_opcode = TRUE;

    }

    return translated_opcode;
}

BOOL FirstThirdFourth(unsigned short opcode, size_t opcodes_idx)
{
    BOOL translated_opcode = FALSE;

    int first  = (opcode & 0xF000) == (opcodes[opcodes_idx].opcode & 0xF000);
    int third  = (opcode & 0x00F0) == (opcodes[opcodes_idx].opcode & 0x00F0);
    int fourth = (opcode & 0x000F) == (opcodes[opcodes_idx].opcode & 0x000F);

    if (first && third && fourth) {

        opcodes[opcodes_idx].translate(opcode);
        translated_opcode = TRUE;

    }

    return translated_opcode;
}

void DebugTranslateSingleInstruction(unsigned short opcode) 
{

    SetupNextTranslation(opcode);

}

BOOL StubOpcode(unsigned short opcode) 
{
    BOOL translated_opcode = FALSE;
    printf("Reached stub function with opcode %04x\n", opcode);
    return translated_opcode;

}

void ClearScreen(unsigned short opcode) 
{

}

void Return(unsigned short opcode) 
{

}

void CallProg(unsigned short opcode) 
{

}

void Jump(unsigned short opcode) 
{

}

void CallSubroutine(unsigned short opcode) 
{

}

void Skip_IfVx_EqualNN(unsigned short opcode) 
{

}

void Skip_IfVx_DoesNotEqualNN(unsigned short opcode) 
{

}

void Skip_IfVx_Equal_Vy(unsigned short opcode) 
{

}

void Set_Vx_To_NN(unsigned short opcode) 
{

}

void Add_NN_To_Vx(unsigned short opcode) 
{

}

// Set Vx To Vy
// 8XY0 Sets VX to value of VY
void Vx_ToVy(unsigned short opcode) 
{
	
	unsigned char x = (unsigned char)((0x0F00 & opcode) >> 8);
	unsigned char y = (unsigned char)((0x00F0 & opcode) >> 4);

	v_regs[x] = v_regs[y];	

}

void Vx_To_VxOrVy(unsigned short opcode) 
{

}

void Vx_To_VxAndVy(unsigned short opcode) 
{

}

void Vx_To_VxXorVy(unsigned short opcode) 
{

}

void Add_Vy_To_VxCarry(unsigned short opcode) 
{

}

void Sub_Vy_From_VxBorrow(unsigned short opcode) 
{

}

void Store_Least_Sig_Shift_Right(unsigned short opcode) 
{

}

void Vx_To_VyMinusVxBorrow(unsigned short opcode) 
{

}

void Store_Most_Sig_Shift_Left(unsigned short opcode) 
{

}

void SkipNextIf_VxDoesNotEqual_Vy(unsigned short opcode) 
{

}

void Set_I_to_NN(unsigned short opcode) 
{

}

void Jump_ToAdressNNPlusV0(unsigned short opcode) 
{

}

void SetVx_To_RandomAndNN(unsigned short opcode) 
{

}

void DrawSrite(unsigned short opcode) 
{

}

void Skip_Instruction_If_Key_IsPressed(unsigned short opcode) 
{

}

void Skip_Instruction_If_Key_Not_Pressed(unsigned short opcode) 
{

}

void SetVXToDelayTimer(unsigned short opcode) 
{

}

void GetBlockingKeyPress(unsigned short opcode) 
{

}

void SetDelayTimer(unsigned short opcode) 
{

}

void SetSoundTimer(unsigned short opcode) 
{

}

void AddVx_To_I(unsigned short opcode) 
{

}

void Set_I_ToLocationOfSprite(unsigned short opcode) 
{

}

void StoreVx(unsigned short opcode) 
{

}

void StoreAllVs(unsigned short opcode) 
{

}

void FillAllVs(unsigned short opcode) 
{

}
