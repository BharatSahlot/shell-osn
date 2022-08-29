#include "cmds.h"
#include "../logger.h"
#include "../utils.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

extern char path[250];
extern char absolute_path[250];

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
            return;
        }
        int n = strlen(path);
        for(int i = n - 1; i >= 0; --i)
        {
            if(path[i] == '/')
            {
                path[i] = '\0';
                break;
            }
        }
        return;
    }

    // cd into folder
    int status = checkIfDirectoryExists(args);
    if(status == 0)
    {
        Log(LOGL_ERROR, "cd: %s is not a directory\n", args);
    } else if(status == 1)
    {
        int n = strlen(path);
        path[n] = '/';
        strcpy(path + n + 1, args);
    } else if(status == -1)
    {
        LogPError("cd: error cding into directory");
    }
}
