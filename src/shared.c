#include <stdio.h>
#include <time.h>
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

long diff_time_with_now_in_nano(struct timespec some_time) 
{
    static const long NANO_IN_SEC = 1e9;

    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);

    return (now.tv_sec - some_time.tv_sec) * NANO_IN_SEC + (now.tv_nsec - some_time.tv_nsec);

}

long diff_time_with_now_in_mill(struct timespec some_time) 
{

    static const long NANO_IN_MILLI = 1e3;

    long nano_diff = diff_time_with_now_in_nano(some_time);

    return nano_diff / NANO_IN_MILLI;

}