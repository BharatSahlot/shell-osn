#include "cmds.h"

#include <stdio.h>
#include <unistd.h>

extern char path[250];
extern char home_path[250];

char dir[250];

void pwd(char *args)
{
    getcwd(dir, 250);
    printf("%s\n", dir);
}
