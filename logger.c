#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void Log(int level, const char *format, ...)
{
    char buffer[512];
    switch (level) {
        case 0:
            strcpy(buffer, "\033[39m");
            break;
        case 1:
            strcpy(buffer, "\033[33m");
            break;
        case 2:
            strcpy(buffer, "\033[31m");
            break;
    }

    va_list args;
    va_start(args, format);
    vsprintf(buffer + 5, format, args);

    if(level == LOGL_ERROR) perror(buffer);
    else printf("%s\n", buffer);

    va_end(args);
}
