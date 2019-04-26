#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned char memory[4096];

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

void SkipNextInstruction() 
{

	printf("SkipNextInstruction STUB");

}

unsigned short GetNextOpCode() 
{
    return 0;
}
