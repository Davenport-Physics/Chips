#include <stdio.h>
#include <stdarg.h>

FILE *fp;

void InitializeFilePointerIfNeeded() 
{
    if (fp == NULL) {
        fp = fopen("debug.log", "w");
    } 
}

void DebugLog(const char *format, ...) 
{

    InitializeFilePointerIfNeeded();
    
    va_list args;
    va_start(args, format);
    vfprintf(fp, format, args);
    va_end(args);

}