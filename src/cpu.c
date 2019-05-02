#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "memory.h"
#include "controls.h"
#include "shared.h"
#include "draw.h"
#include "sound.h"

// from left to right.
static const uint_16 FIRST  = 1;
static const uint_16 SECOND = 2;
static const uint_16 THIRD  = 4;
static const uint_16 FOURTH = 8;

static const uint_16 ALL_NIBS = FIRST | SECOND | THIRD | FOURTH;
static const uint_16 FF_NIBS  = FIRST | FOURTH;
static const uint_16 FTF_NIBS = FIRST | THIRD | FOURTH;

static uint_8 v_regs[16];
static uint_16 I_reg          = 0;
static uint_16 sound_timer    = 0;
static uint_16 delay_timer    = 0;
static uint_16 current_opcode = 0;

BOOL First(uint_16 opcode, size_t opcodes_idx);
BOOL ExactOpcode(uint_16 opcode, size_t opcodes_idx);
BOOL FirstFourth(uint_16 opcode, size_t opcodes_idx);
BOOL FirstThirdFourth(uint_16 opcode, size_t opcodes_idx);

uint_8 GetNibble(uint_16 opcode, uint_16 nibble);
uint_16 GetSigDecimal(char, int);

/* Register functions */
void ClearScreen(uint_16);
void Return(uint_16);
void CallProg(uint_16);
void Jump(uint_16);
void CallSubroutine(uint_16);

void Skip_IfVx_EqualNN(uint_16);
void Skip_IfVx_DoesNotEqualNN(uint_16);
void Skip_IfVx_Equal_Vy(uint_16);

void Set_Vx_To_NN(uint_16);
void Add_NN_To_Vx(uint_16);

void Vx_ToVy(uint_16);
void Vx_To_VxOrVy(uint_16);
void Vx_To_VxAndVy(uint_16);
void Vx_To_VxXorVy(uint_16);

void Add_Vy_To_VxCarry(uint_16);
void Sub_Vy_From_VxBorrow(uint_16);
void Store_Least_Sig_Shift_Right(uint_16);

void Vx_To_VyMinusVxBorrow(uint_16);
void Store_Most_Sig_Shift_Left(uint_16);
void SkipNextIf_VxDoesNotEqual_Vy(uint_16);
void Set_I_to_NN(uint_16);
void Jump_ToAddressNNPlusV0(uint_16);
void SetVx_To_RandomAndNN(uint_16);
void DrawSprite(uint_16);
void Skip_Instruction_If_Key_IsPressed(uint_16);
void Skip_Instruction_If_Key_Not_Pressed(uint_16);
void SetVXToDelayTimer(uint_16);
void GetBlockingKeyPress(uint_16);
void SetDelayTimer(uint_16);
void SetSoundTimer(uint_16);
void AddVx_To_I(uint_16);
void Set_I_ToLocationOfSprite(uint_16);
void StoreVx(uint_16);
void StoreAllVs(uint_16);
void FillAllVs(uint_16);

void SetupNextTranslation(uint_16);

struct opcode {
    uint_16 opcode;
    void (*translate)(uint_16);
    uint_16 necessary_nibbles;
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
    {0xB000, &Jump_ToAddressNNPlusV0             , FIRST}, // BNNN Jumps to the address [NNN + V0]
    {0xC000, &SetVx_To_RandomAndNN               , FIRST}, // CXNN Sets VX to the result of a bitwise AND operation on a random number 0-255 and NN
    {0xD000, &DrawSprite                          , FIRST}, // DXYN Draws a sprite at (VX, VY) with width of 8 and N height in N pixels.
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
    srand(time(NULL));

}

void ExecuteNextOpCode()
{
    SetupNextTranslation(GetNextOpCode());

    if (delay_timer > 0) {

        delay_timer--;

    }

    if (sound_timer > 0) {

        if (sound_timer == 1) {

            PlayBeep();

        }
        sound_timer--;

    }
}

void LogRegisters() 
{

    for (size_t i = 0; i < 16; i++) {

        DebugLog("V%02x = %02x\n", i, v_regs[i]);

    }
    DebugLog("I = %04x\n", I_reg);

}

void SetupNextTranslation(uint_16 opcode)
{

    DebugLog("Current opcode = %04x\n", opcode);
    BOOL translation_success = FALSE;
    for (int i = 0; i < 35; i++) {

		if (opcodes[i].necessary_nibbles == FIRST) 
			translation_success = First(opcode, i);
        else if (opcodes[i].necessary_nibbles == ALL_NIBS)
			translation_success = ExactOpcode(opcode, i);
        else if (opcodes[i].necessary_nibbles == FF_NIBS) 
			translation_success = FirstFourth(opcode, i);
		else if (opcodes[i].necessary_nibbles == FTF_NIBS)
			translation_success = FirstThirdFourth(opcode, i);

        if (translation_success)
            break;

    }
    LogRegisters();
    DebugLog("-------\n");

}

BOOL First(uint_16 opcode, size_t opcodes_idx) 
{
    BOOL translated_opcode = FALSE;

    if ((opcode & 0xF000) == (opcodes[opcodes_idx].opcode & 0xF000)) {

        opcodes[opcodes_idx].translate(opcode);
        translated_opcode = TRUE;

    }

    return translated_opcode;
}

BOOL ExactOpcode(uint_16 opcode, size_t opcodes_idx) 
{
    BOOL translated_opcode = FALSE;

    if ((opcode ^ opcodes[opcodes_idx].opcode) == 0) {

        opcodes[opcodes_idx].translate(opcode);
        translated_opcode = TRUE;

    }

    return translated_opcode;
}

BOOL FirstFourth(uint_16 opcode, size_t opcodes_idx) 
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

BOOL FirstThirdFourth(uint_16 opcode, size_t opcodes_idx)
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

void DebugTranslateSingleInstruction(uint_16 opcode) 
{

    SetupNextTranslation(opcode);

}

BOOL StubOpcode(uint_16 opcode) 
{
    BOOL translated_opcode = FALSE;
    DebugLog("Reached stub function with opcode %04x\n", opcode);
    return translated_opcode;

}

void ClearScreen(uint_16 opcode) 
{

    DebugLog("Clearing Screen");
    ClearDrawScreen();

}

void Return(uint_16 opcode) 
{
    SetReturnAddress();
}

void CallProg(uint_16 opcode) 
{

    DebugLog("CallProg STUB. opcode = %04x\n", opcode);

}

// 1NNN
void Jump(uint_16 opcode) 
{

    JumpToInstruction(0x0FFF & opcode);

}

void CallSubroutine(uint_16 opcode) 
{
    CallIntruction(0x0FFF & opcode);
}

void Skip_IfVx_EqualNN(uint_16 opcode) 
{

	uint_8 x = GetNibble(opcode, 2);
	uint_8 nn = (uint_8)(0x00FF & opcode);

	if (v_regs[x] == nn) {

		SkipNextInstruction();		

	}

}

void Skip_IfVx_DoesNotEqualNN(uint_16 opcode) 
{

	uint_8 x = GetNibble(opcode, 2);
	uint_8 nn = (uint_8)(0x00FF & opcode);

	if (v_regs[x] != nn) {
	
		SkipNextInstruction();

	}	

}

void Skip_IfVx_Equal_Vy(uint_16 opcode) 
{

	uint_8 x = GetNibble(opcode, 2);	
	uint_8 y = GetNibble(opcode, 3);

	if (v_regs[x] == v_regs[y]) {
	
		SkipNextInstruction();
	
	}

}

void Set_Vx_To_NN(uint_16 opcode) 
{

	uint_8 x  = GetNibble(opcode, 2);
	uint_8 nn = (uint_8)(0x00FF & opcode);

	v_regs[x] = nn;

}

void Add_NN_To_Vx(uint_16 opcode) 
{

	uint_8 x  = GetNibble(opcode, 2);
	uint_8 nn = (uint_8)(0x00FF & opcode);

	v_regs[x] += nn;

}

// Set Vx To Vy
// 8XY0 Sets VX to value of VY
void Vx_ToVy(uint_16 opcode) 
{
	
	uint_8 x = GetNibble(opcode, 2);
	uint_8 y = GetNibble(opcode, 3);

	v_regs[x] = v_regs[y];	

}

void Vx_To_VxOrVy(uint_16 opcode) 
{

	uint_8 x = GetNibble(opcode, 2);
	uint_8 y = GetNibble(opcode, 3);

	v_regs[x] = v_regs[x] | v_regs[y];

}

void Vx_To_VxAndVy(uint_16 opcode) 
{

	uint_8 x = GetNibble(opcode, 2);
	uint_8 y = GetNibble(opcode, 3);

	v_regs[x] = v_regs[x] & v_regs[y];

}

void Vx_To_VxXorVy(uint_16 opcode) 
{

	uint_8 x = GetNibble(opcode, 2);
	uint_8 y = GetNibble(opcode, 3);

	v_regs[x] = v_regs[x] ^ v_regs[y];

}

void Add_Vy_To_VxCarry(uint_16 opcode) 
{

	uint_8 x = GetNibble(opcode, 2);
	uint_8 y = GetNibble(opcode, 3);

    short temp = (short)v_regs[x] + (short)v_regs[y];
    if (temp > 0xFF)
        v_regs[15] = 1;
    else
        v_regs[15] = 0;

	v_regs[x] += v_regs[y];	

}

void SetVFIfBorrowing(char x, char y) 
{

    if (x < y)
        v_regs[15] = 0;
    else
        v_regs[15] = 1;

}

void Sub_Vy_From_VxBorrow(uint_16 opcode) 
{

	uint_8 x = GetNibble(opcode, 2);
	uint_8 y = GetNibble(opcode, 3);

    SetVFIfBorrowing(v_regs[x], v_regs[y]);
	v_regs[x] -= v_regs[y];

}

// 8XY6
void Store_Least_Sig_Shift_Right(uint_16 opcode) 
{

    uint_8 x   = GetNibble(opcode, 2);
    v_regs[15] = 0x01 | v_regs[x];
    v_regs[x]  = v_regs[x] >> 1;

}

// 8XY7
void Vx_To_VyMinusVxBorrow(uint_16 opcode) 
{

	uint_8 x = GetNibble(opcode, 2);
	uint_8 y = GetNibble(opcode, 3);

	SetVFIfBorrowing(v_regs[y], v_regs[x]);
	v_regs[x] = v_regs[y] - v_regs[x];

}

void Store_Most_Sig_Shift_Left(uint_16 opcode) 
{

    uint_8 x = GetNibble(opcode, 2);
    v_regs[15]      = 0x80 | v_regs[x];
    v_regs[x]       = v_regs[x] << 1;

}

// 9XY0
void SkipNextIf_VxDoesNotEqual_Vy(uint_16 opcode) 
{

    uint_8 x = GetNibble(opcode, 2);
    uint_8 y = GetNibble(opcode, 3);

    if (v_regs[x] != v_regs[y]) {

        SkipNextInstruction();

    }

}

// ANNN Sets I to the address of NNN
void Set_I_to_NN(uint_16 opcode) 
{

	uint_16 nnn = 0x0FFF & opcode;
	I_reg = nnn;

}

// BNNN
void Jump_ToAddressNNPlusV0(uint_16 opcode) 
{

    uint_16 nnn = 0x0FFF & opcode;
    JumpToInstruction(nnn + v_regs[0]);

}

// CXNN
void SetVx_To_RandomAndNN(uint_16 opcode) 
{

    uint_8 x  = GetNibble(opcode, 2);
    uint_8 nn = (uint_8)(0x00FF & opcode);
    uint_8 rn = rand()%256;

    v_regs[x] = rn & nn;

}

void HandleCollisions() 
{

    if(CollisionDetected()) {

        v_regs[15] = 1;

    } else {

        v_regs[15] = 0;

    }

}

void PrintBitsToDraw(uint_8 bits_to_draw) 
{

    char bits[9];
    bits[8] = '\0';
    for (size_t i = 0; i < 8; i++) {

        if ((bits_to_draw << i) & 0x80) {

            bits[i] = '1';

        } else {

            bits[i] = '0';

        }

    }
    DebugLog("%s\n", bits);

}

// DXYN
void DrawSprite(uint_16 opcode) 
{
    uint_8 x = v_regs[GetNibble(opcode, 2)];
    uint_8 y = v_regs[GetNibble(opcode, 3)];
    uint_8 n = GetNibble(opcode, 4);

    uint_8 bits_to_draw[n];
    DebugLog("DRAWING\n");
    for (size_t i = 0; i < n;i++) {

        bits_to_draw[i] = GetValueAtAddress(I_reg + i);
        PrintBitsToDraw(bits_to_draw[i]);
    }
    DebugLog("END_DRAWING\n");
    DrawPixels(x, y, bits_to_draw, n);
    HandleCollisions();


}

// EX9E
void Skip_Instruction_If_Key_IsPressed(uint_16 opcode) 
{

    DebugLog("IsKeyPressed = %02x\n", v_regs[GetNibble(opcode, 2)]);
	if (IsKeyPressed(v_regs[GetNibble(opcode, 2)])) {
	
        DebugLog("Skipping next instruction because IsKeyPressed\n");
		SkipNextInstruction();	
	
	}

}

void Skip_Instruction_If_Key_Not_Pressed(uint_16 opcode) 
{

    DebugLog("IsNotKeyPressed = %02x\n", v_regs[GetNibble(opcode, 2)]);
	if (!IsKeyPressed(v_regs[GetNibble(opcode, 2)])) {
	
        DebugLog("Skipping next instruction because IsNotKeyPressed\n");
		SkipNextInstruction();
	
	}

}

// FX07
void SetVXToDelayTimer(uint_16 opcode) 
{

    v_regs[GetNibble(opcode, 2)] = delay_timer;

}

void GetBlockingKeyPress(uint_16 opcode) 
{

    DebugLog("Awaiting keypress\n");
    v_regs[GetNibble(opcode, 2)] = AwaitKeyPress();

}

// FX15
void SetDelayTimer(uint_16 opcode) 
{

    delay_timer = v_regs[GetNibble(opcode, 2)];

}

// FX18
void SetSoundTimer(uint_16 opcode) 
{

    sound_timer = v_regs[GetNibble(opcode, 2)];

}

// FX1E
void AddVx_To_I(uint_16 opcode) 
{

	I_reg += v_regs[GetNibble(opcode, 2)];

}

// FX29 sets I to the location of the sprite for the character in VX. (Chars 0-F are represented by 4x5 font)
void Set_I_ToLocationOfSprite(uint_16 opcode) 
{

    I_reg = v_regs[GetNibble(opcode, 2)] * 5;

}

// FX33
void StoreVx(uint_16 opcode) 
{

    uint_8 x = GetNibble(opcode, 2);
    for (int i = 2; i >= 0; i--) {

        SetValueAtAddress(GetSigDecimal(v_regs[x], i), I_reg);
        I_reg += 2;

    }


}

// FX55 Stores V0 to VX in memory starting at address I
void StoreAllVs(uint_16 opcode) 
{

	size_t x           = GetNibble(opcode, 2);
    uint_16 I_reg_temp = I_reg;

    for (size_t i = 0;i < x; i++) {

		SetValueAtAddress(v_regs[i], I_reg_temp);
		I_reg_temp++;

    }

}

void FillAllVs(uint_16 opcode) 
{

	uint_8 x           = GetNibble(opcode, 2);
    uint_16 I_reg_temp = I_reg;

    for (int i = x; i >= 0; i--) {

        v_regs[i] = GetValueAtAddress(I_reg_temp);
		I_reg_temp--;

    }

}

uint_8 GetNibble(uint_16 opcode, uint_16 nibble) 
{

	switch (nibble) {
	
	case 1:
		return (uint_8)((0xF000 & opcode) >> 12);
	break;

	case 2:
		return (uint_8)((0x0F00 & opcode) >> 8);
	break;

	case 3:
		return (uint_8)((0x00F0 & opcode) >> 4);
	break;

	case 4:
		return (uint_8)((0x000F & opcode));
	break;	

	}

	DebugLog("ERROR: nibble was passed with unusable value %d", nibble);
	exit(0);
	return 0;

}

uint_16 GetSigDecimal(char value, int place) 
{

    char denominator = pow(10, place);
    char sig_digit   = abs(value) / denominator;

    return sig_digit % 10;

}
