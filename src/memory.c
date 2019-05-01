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
static uint_16 mem_pointer   = 512;
static uint_16 stack_pointer = 0x0EA0;

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

        printf("%s\n", argv[1]);
        ReadChipFile(argv[1]); // filename at argv[1]

    } else {

        printf("You did not pass a file to be read\n");
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
    int i = 0;

    while ((c = getc(fp)) != EOF)  {
        
        memory[512 + i] = (char)c;
        i++;

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

uint_16 pop(int num_bytes) 
{

    uint_16 value = 0;

    if (stack_pointer < 0x0EA0) {

        printf("Stack underflow detected\n");
        exit(0);

    }
    if (num_bytes > 2) {

        printf("May only pop 2 bytes or less\n");
        exit(0);

    }
    for (int i = 0;i < num_bytes;i++) {

        stack_pointer--;
        value = value | (memory[stack_pointer] << 8*i);

    }
    return value;

}

void push(uint_16 word) 
{

    uint_16 high_byte = ((word & 0xFF00) >> 8);
    uint_16 low_byte  = (word & 0x00FF);

    if (stack_pointer < 0x0EFF) {

        memory[stack_pointer] = high_byte;
        stack_pointer++;
        memory[stack_pointer] = low_byte;
        stack_pointer++;

    } else {

        printf("Stack overflow detected.\n");
        exit(0);

    }

}

void CheckMemPointerOutOfBounds() 
{

    if (mem_pointer >= 4096) {

        printf("mem_pointer is out of bounds\n");
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
    push(mem_pointer+2);
    mem_pointer = location;

}

void SetReturnAddress() 
{
    mem_pointer = pop(2);
}

uint_16 GetNextOpCode() 
{

    CheckMemPointerOutOfBounds();
    uint_16 opcode = (memory[mem_pointer] << 8) | memory[mem_pointer+1];
    mem_pointer += 2;
    return opcode;
    
}

void SetValueAtAddress(char value, uint_16 address) 
{

	memory[address] = value;

}

char GetValueAtAddress(uint_16 address) 
{

	return memory[address];

}
