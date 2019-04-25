#ifndef CPU_H
#define CPU_H

void InitializeCPU();
void ExecuteNextOpCode();
void DebugTranslateSingleInstruction(unsigned short opcode);

#endif