#include "cmds.h"

#include <stdio.h>

void echo(int argc, char* argv[])
{
    for(int i = 1; i < argc; i++)
    {
        if(i > 1) printf(" ");
        printf("%s", argv[i]);
    }
    printf("\n");
}
