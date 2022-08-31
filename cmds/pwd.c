#include "cmds.h"
#include "../logger.h"

#include <stdio.h>
#include <unistd.h>

extern char path[250];
extern char home_path[250];

char dir[250];

void pwd(int argc, char* argv[])
{
    if(argc > 1)
    {
        Log(LOGL_ERROR, "pwd: too many arguments\n");
        return;
    }
    getcwd(dir, 250);
    printf("%s\n", dir);
}
