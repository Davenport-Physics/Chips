#ifndef SHARED_H
#define SHARED_H

#include <stdarg.h>

typedef enum BOOL {

    FALSE = 0,
    TRUE

} BOOL;

typedef char int_8;
typedef unsigned char uint_8;
typedef unsigned short uint_16;

void DebugLog(const char *, ...);
void PrintBitsToDraw(uint_8 bits_to_draw);

#endif
