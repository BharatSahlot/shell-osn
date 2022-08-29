#include "cmds.h"
#include "../logger.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>

extern char path[50];
extern char absolute_path[50];

void cd(char *args)
{
    args[strlen(args)] = '\0';

    int argc = 0;
    char* pc = strtok(args, " \n");
    while(pc != NULL)
    {
        if(strlen(pc) > 0)
        {
            // printf("%s\n", pc);
            argc++;
        }

        pc = strtok(NULL, " \n");
    }
    if(argc > 1)
    {
        Log(LOGL_ERROR, "cd: Expected 1 arg found %d\n", argc);
        return;
    }

    if(strcmp(args, ".") == 0) return;
    if(strcmp(args, "-") == 0)
    {
        printf("%s\n", path);
        return;
    }
    if(strcmp(args, "..") == 0)
    {
        if(strcmp(path, "~") == 0)
        {
            printf("%s\n", absolute_path);
        }
    }
}
