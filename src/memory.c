#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"

static uint_8 memory[4096] = {

  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F

};
static uint_16 mem_pointer    = 512;
static uint_16 stack_pointer  = 0x0EA0;
static uint_16 last_byte      = 512;
static uint_16 current_opcode = 0; 

void SetupReadOrExit(int argc, char **argv);
void ReadChipFile(char *file_name);
void CheckMemPointerOutOfBounds();
void ResetMemory();
void DumpContentsOfMemoryToFile();

void InitializeMemory(int argc, char **argv)
{

    ResetMemory();
    SetupReadOrExit(argc, argv);
    DumpContentsOfMemoryToFile();

}

void SetupReadOrExit(int argc, char **argv) 
{

    if (argc > 1) {

        DebugLog("%s\n", argv[1]);
        ReadChipFile(argv[1]); // filename at argv[1]

    } else {

        DebugLog("You did not pass a file to be read\n");
        exit(0);

    }

}

void ResetMemory() 
{

    memset(memory+80, 0, 4096-80);

}

void ReadChipFile(char *file_name) 
{

    FILE *fp = fopen(file_name, "rb");
    
    short c;
    while ((c = getc(fp)) != EOF)  {
        
        memory[last_byte] = (char)c;
        last_byte++;

    }

    fclose(fp);

}

void DumpContentsOfMemoryToFile() 
{

    FILE *fp = fopen("chips-dump.dat", "w+");
    for (int i = 0; i < 4096; i += 4) {

        fprintf(fp, "%02x%02x %02x%02x\n", memory[i], memory[i+1], memory[i+2], memory[i+3]);

    }

    fclose(fp);

}

uint_16 Pop(int num_bytes) 
{

    uint_16 value = 0;

    if (stack_pointer < 0x0EA0) {

        DebugLog("Stack underflow detected\n");
        exit(0);

    }
    if (num_bytes > 2) {

        DebugLog("May only Pop 2 bytes or less\n");
        exit(0);

    }
    for (int i = 0;i < num_bytes;i++) {

        stack_pointer--;
        value = value | (memory[stack_pointer] << 8*i);

    }
    return value;

}

void Push(uint_16 word) 
{

    uint_16 high_byte = ((word & 0xFF00) >> 8);
    uint_16 low_byte  = (word & 0x00FF);

    DebugLog("call_push = %02x%02x\n", high_byte, low_byte);
    if (stack_pointer < 0x0EFF) {

        memory[stack_pointer] = high_byte;
        stack_pointer++;
        memory[stack_pointer] = low_byte;
        stack_pointer++;

    } else {

        DebugLog("Stack overflow detected.\n");
        exit(0);

    }

}

void CheckMemPointerOutOfBounds() 
{

    DebugLog("mem_pointer = %d, last_byte = %d\n", mem_pointer, last_byte);
    if (mem_pointer >= 4096 || mem_pointer == last_byte) {

        DebugLog("mem_pointer is out of bounds\n");
        exit(0);

    }

}

void SkipNextInstruction() 
{

    mem_pointer += 2;
    CheckMemPointerOutOfBounds();

}

void JumpToInstruction(uint_16 location) 
{

    mem_pointer = location;

}

void CallIntruction(uint_16 location) 
{
    DebugLog("call_current_opcode = %04x\n", current_opcode);
    DebugLog("call_return_opcode = %04x\n", (memory[mem_pointer] << 8) | memory[mem_pointer+1]);
    Push(mem_pointer);
    mem_pointer = location;

}

void SetReturnAddress() 
{
    mem_pointer = Pop(2);
    DebugLog("return_address = %04x\n", mem_pointer);
}

uint_16 GetNextOpCode() 
{

    CheckMemPointerOutOfBounds();
    current_opcode = (memory[mem_pointer] << 8) | memory[mem_pointer+1];
    mem_pointer  += 2;

    return current_opcode;
    
}

void SetValueAtAddress(char value, uint_16 address) 
{

	memory[address] = value;

}

// TODO check is char is appropriate here.
uint_8 GetValueAtAddress(uint_16 address) 
{

	return memory[address];

}
