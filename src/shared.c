#include <stdio.h>
#include <stdarg.h>

#include "shared.h"

FILE *fp;

void InitializeFilePointerIfNeeded() 
{
    if (fp == NULL) {
        fp = fopen("debug.log", "w");
    } 
}

void DebugLog(const char *format, ...) 
{

#if DEBUGGING

    InitializeFilePointerIfNeeded();
    
    va_list args;
    va_start(args, format);
    vfprintf(fp, format, args);
    va_end(args);

#endif

}

void PrintBitsToDraw(uint_8 bits_to_draw) 
{

#if DEBUGGING

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
    
#endif

}