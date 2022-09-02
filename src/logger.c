#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

char errBuf[512];
void Log(int level, const char *format, ...)
{
    switch (level) {
        case 0:
            strcpy(errBuf, "\033[39m");
            break;
        case 1:
            strcpy(errBuf, "\033[33m");
            break;
        case 2:
            strcpy(errBuf, "\033[31m");
            break;
    }

    va_list args;
    va_start(args, format);
    int len = vsprintf(errBuf + 5, format, args);

    if(level == LOGL_ERROR) fprintf(stderr, "%s\033[0m", errBuf);
    else printf("%s\033[0m", errBuf);

    va_end(args);
}


void LogPError(const char *format, ...)
{
    // strcpy(errBuf, "\033[31m");
    fprintf(stderr, "\033[31m");

    va_list args;
    va_start(args, format);
    vsprintf(errBuf, format, args);
    perror(errBuf);
    fprintf(stderr, "\033[0m");
    va_end(args);
}
