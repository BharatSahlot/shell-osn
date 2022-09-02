#include "builtins.h"

#include <stdio.h>

int history(int argc, const char* argv[])
{
    if(argc > 1)
    {
        Log(LOGL_WARN, "history: too many arguements\n");
    }
    int i = historyCount - HCOMMANDS_TO_DISPLAY;
    if(i < 0) i = 0;
    for(; i < historyCount; i++)
    {
        printf("%s\n", historyArr[i]);
    }
    return 0;
}
