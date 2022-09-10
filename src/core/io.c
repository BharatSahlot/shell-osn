#include "io.h"

#include <stdarg.h>
#include <stdio.h>

#define BUFFER_SIZE 10000

int linesPrinted = 0;

void print(const char* format, ...)
{
    static char buffer[BUFFER_SIZE];
    va_list args;
    va_start(args, format);
    int n = vsprintf(buffer, format, args);

    printf("%s", buffer);
    fflush(stdout);

    for(int i = 0; i < n; i++)
    {
        if(buffer[i] == '\n') linesPrinted++;
    }
    va_end(args);
}

int getPrintedLinesCount()
{
    return linesPrinted;
}
