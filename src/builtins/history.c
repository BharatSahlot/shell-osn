#include "builtins.h"

#include <stdio.h>

int history(int argc, const char* argv[])
{
    if(argc > 1)
    {
        Log(LOGL_WARN, "history: too many arguements\n");
    }
    for(int i = 0; i < historyCount; i++)
    {
        printf("%s\n", historyArr[i]);
    }
    return 0;
}
