#include "builtins.h"

#include <stdio.h>

int pwd(int argc, const char **argv)
{
    if(argc > 1)
    {
        Log(LOGL_WARN, "pwd: too many arguements\n");
    }
    printf("%s\n", currentPath);
    return 0;
}
