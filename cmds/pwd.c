#include "cmds.h"

#include <stdio.h>

extern char path[250];
extern char absolute_path[250];

void pwd(char *args)
{
    printf("%s%s\n", absolute_path, path + 1);
}
