#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char memory[4096];
static unsigned short mem_pointer = 512;

static unsigned short stack_pointer = 0x0EA0;

void SetupReadOrExit(int argc, char **argv);
void ReadChipFile(char *file_name);
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

        printf("\nYou did not pass a file to be read\n");
        exit(0);

    }

}

void ResetMemory() 
{

    memset(memory, 0, 4096);

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

unsigned short pop(int num_bytes) 
{

    unsigned short value = 0;

    if (stack_pointer < 0x0EA0) {

        printf("\nStack underflow detected\n");
        exit(0);

    }
    if (num_bytes > 2) {

        printf("\nMay only pop 2 bytes or less\n");
        exit(0);

    }
    for (int i = 0;i < num_bytes;i++) {

        stack_pointer--;
        value = value | (memory[stack_pointer] << 8*i);

    }
    return value;

}

void push(unsigned short word) 
{

    unsigned short high_byte = ((word & 0xFF00) >> 8);
    unsigned short low_byte  = (word & 0x00FF);

    if (stack_pointer < 0x0EFF) {

        memory[stack_pointer] = high_byte;
        stack_pointer++;
        memory[stack_pointer] = low_byte;
        stack_pointer++;

    } else {

        printf("\nStack overflow detected.\n");
        exit(0);

    }

}

void SkipNextInstruction() 
{

    mem_pointer += 2;
    if (mem_pointer > 4096) {

        printf("\nmem_pointer is beyond scope\n");
        exit(0);

    }

}

void JumpToInstruction(unsigned short location) 
{

    mem_pointer = location;

}

void CallIntruction(unsigned short location) 
{
    push(mem_pointer+2);
    mem_pointer = location;

}

void SetReturnAddress() 
{
    mem_pointer = pop(2);
}

unsigned short GetNextOpCode() 
{
    unsigned short opcode = (memory[mem_pointer] << 8) | memory[mem_pointer+1];
    mem_pointer += 2;
    return opcode;
}
