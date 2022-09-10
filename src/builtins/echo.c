#include "builtins.h"

#include <stdio.h>

int echo(int argc, const char **argv)
{
    for(int i = 1; i < argc; i++)
    {
        if(i > 1) print(" ");
        print("%s", argv[i]);
    }
    print("\n");
    return 0;
}
