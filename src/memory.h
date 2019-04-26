#ifndef MEMORY_H
#define MEMORY_H

void InitializeMemory(int argc, char **argv);
void SkipNextInstruction();
void JumpToInstruction(unsigned short);
void CallIntruction(unsigned short);

void push(unsigned short);
unsigned short pop(int num_bytes);

unsigned short GetNextOpCode();

#endif
