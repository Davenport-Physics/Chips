#ifndef MEMORY_H
#define MEMORY_H

#include "shared.h"

void InitializeMemory(int argc, char **argv);
void SkipNextInstruction();
void JumpToInstruction(unsigned short);
void CallIntruction(unsigned short);
void SetReturnAddress();

void push(unsigned short);
uint_16 pop(int num_bytes);

uint_16 GetNextOpCode();
void SetValueAtAddress(unsigned short value, unsigned short address);
uint_16 GetValueAtAddress(unsigned short address);

#endif
