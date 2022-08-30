#include "cmds.h"
#include "../logger.h"
#include "../utils.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

extern char path[250];
extern char home_path[250];
extern char absolute_path[250];

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
        strcpy(path, "~");
        strcpy(absolute_path, home_path);
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
        strcpy(path, "~");
        strcpy(absolute_path, home_path);
        return;
    }
    if(strcmp(pc, "..") == 0)
    {
        if(strcmp(path, "~") == 0)
        {
            printf("%s\n", home_path);
            return;
        }
        moveUpDirectory(path);
        moveUpDirectory(absolute_path);
        return;
    }

    // cd into folder
    char* dir = pc;
    if(*dir == '~' && *(dir + 1) == '/')
    {
        dir += 2;
    }

    // so that paths like ~/.. are ignored
    if(strcmp(dir, "..") == 0 || strcmp(dir, ".") == 0 || strcmp(dir, "~") == 0)
    {
        Log(LOGL_ERROR, "cd: %s not a directory\n", pc);
        return;
    }

    joinPaths(absolute_path, dir);
    int status = checkIfDirectoryExists(absolute_path);
    moveUpDirectory(absolute_path);
    if(status == 0)
    {
        Log(LOGL_ERROR, "cd: %s is not a directory\n", pc);
    } else if(status == 1)
    {
        joinPaths(path, dir);
        joinPaths(absolute_path, dir);
    } else if(status == -1)
    {
        LogPError("cd: error cding into directory %s", pc);
    }
}
