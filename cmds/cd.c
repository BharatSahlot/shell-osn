#include "cmds.h"
#include "../logger.h"
#include "../utils.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

extern char path[250];
extern char home_path[250];

char prevPath[250];

void formatPath(char* path)
{
    if(strncmp(path, home_path, strlen(home_path)) == 0)
    {
        int hn = strlen(home_path);
        int pn = strlen(path);

        memmove(path + 1, path + hn,  pn - hn);
        path[0] = '~';
        path[pn - hn + 1] = '\0';
    }
}

void cd(int argc, char* argv[])
{
    if(argc == 1)
    {
        if(chdir(home_path) == -1)
        {
            LogPError("cd: Error cd into ~");
            return;
        }
        strcpy(path, "~");
        return;
    }

    if(argc > 2)
    {
        Log(LOGL_ERROR, "cd: Unsupported number of arguements %d\n", argc - 1);
        return;
    }

    char* flag = argv[1];
    if(strcmp(flag, ".") == 0)
    {
        getcwd(prevPath, 250);
        return;
    }

    if(strcmp(flag, "-") == 0)
    {
        if(strlen(prevPath) == 0) strcpy(prevPath, home_path);
        getcwd(path, 250);
        if(chdir(prevPath) == -1)
        {
            formatPath(path);
            LogPError("cd");
            return;
        }
        strcpy(prevPath, path);
        getcwd(path, 250);
        formatPath(path);
        printf("%s\n", path);
        return;
    }
    if(strcmp(flag, "~") == 0)
    {
        if(chdir(home_path) == -1)
        {
            LogPError("cd: Error cd into ~");
            return;
        }
        strcpy(prevPath, home_path);
        strcpy(path, "~");
        return;
    }

    getcwd(prevPath, 250);
    if(*flag == '~')
    {
        if(*(flag + 1) != '/')
        {
            Log(LOGL_ERROR, "cd: Invalid path\n");
            return;
        }
        char temp[250];
        strcpy(temp, home_path);
        joinPaths(temp, flag + 1);
        if(chdir(temp) == -1)
        {
            LogPError("cd");
            return;
        }
        getcwd(path, 250);
    } else
    {
        if(chdir(flag) == -1)
        {
            LogPError("cd");
            return;
        }
        getcwd(path, 250);
    }
    formatPath(path);
}
