#ifndef SHARED_H
#define SHARED_H

#include <time.h>
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

long diff_time_with_now_in_nano(struct timespec);
long diff_time_with_now_in_mill(struct timespec);

#endif
