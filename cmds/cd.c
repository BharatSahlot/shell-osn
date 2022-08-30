#include "cmds.h"
#include "../logger.h"
#include "../utils.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

extern char path[250];
extern char home_path[250];

void joinPaths(char* p1, char* p2)
{
    int n = strlen(p1);
    p1[n] = '/';
    if(*p2 == '/') ++p2;

    int n1 = strlen(p2);

    // paths shouldn't end with /
    if(p2[n1 - 1] == '/') p2[n1 - 1] = '\0';

    strcpy(p1 + n + 1, p2);
}

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
    if(argc == 0)
    {
        if(chdir(home_path) == -1)
        {
            LogPError("cd: Error cd into ~");
            return;
        }
        strcpy(path, "~");
        return;
    }

    if(argc > 1)
    {
        Log(LOGL_ERROR, "cd: Expected 1 arg found %d\n", argc);
        return;
    }

    pc = strtok(args, " \n");
    if(pc[0] == '/')
    {
        Log(LOGL_ERROR, "cd: absolute paths not supported\n");
        return;
    }
    if(strcmp(pc, ".") == 0) return;
    if(strcmp(pc, "-") == 0)
    {
        printf("%s\n", path);
        return;
    }
    if(strcmp(pc, "~") == 0)
    {
        if(chdir(home_path) == -1)
        {
            LogPError("cd: Error cd into ~");
            return;
        }
        strcpy(path, "~");
        return;
    }

    if(*pc == '~')
    {
        if(*(pc + 1) != '/')
        {
            Log(LOGL_ERROR, "cd: Invalid path\n");
            return;
        }
        char temp[250];
        strcpy(temp, home_path);
        joinPaths(temp, pc + 1);
        if(chdir(temp) == -1)
        {
            LogPError("cd");
            return;
        }
        getcwd(path, 250);
    } else
    {
        if(chdir(pc) == -1)
        {
            LogPError("cd");
            return;
        }
        getcwd(path, 250);
    }
    if(strncmp(path, home_path, strlen(home_path)) == 0)
    {
        int hn = strlen(home_path);
        int pn = strlen(path);

        memmove(path + 1, path + hn,  pn - hn);
        path[0] = '~';
        path[pn - hn + 1] = '\0';
    }
}
