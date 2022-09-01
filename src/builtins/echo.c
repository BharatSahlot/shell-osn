#include "builtins.h"

#include <stdio.h>

int echo(int argc, const char **argv)
{
    for(int i = 1; i < argc; i++)
    {
        if(i > 1) printf(" ");
        printf("%s", argv[i]);
    }
    printf("\n");
    return 0;
}
