#include "cmds.h"

#include <stdio.h>

extern char path[250];
extern char home_path[250];

void pwd(char *args)
{
    printf("%s%s\n", home_path, path + 1);
}
