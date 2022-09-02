#include "builtins.h"
#include "../utils.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// absolute path of the directory
int discoverDir(const char* path, const char* target, int shouldSearchFiles, int shouldSearchDirectory)
{
    DIR* dir = opendir(path);
    if(dir == NULL)
    {
        LogPError("discover");
        return -1;
    }

    struct stat st;

    errno = 0;
    struct dirent* item = readdir(dir);
    char tempPath[MAX_PATH_SIZE];
    while(item != NULL)
    {
        // TODO: fix this
        if(item->d_name[0] == '.')
        {
            item = readdir(dir);
            continue;
        }

        strcpy(tempPath, path);
        joinPaths(tempPath, item->d_name);

        // will follow symbolic links
        if(stat(tempPath, &st) == -1)
        {
            LogPError("discover");
            continue;
        }

        if(S_ISDIR(st.st_mode))
        {
            if(shouldSearchDirectory)
            {
                printf("%s\n", tempPath);
            }
            // recurse
            discoverDir(tempPath, target, shouldSearchFiles, shouldSearchDirectory);
        } else 
        {
        }
        item = readdir(dir);
    }
    closedir(dir);
    if(errno != 0)
    {
        LogPError("discover");
        return -1;
    }
    return 0;
}

int discover(int argc, const char *argv[])
{
    char searchDir[MAX_PATH_SIZE] = "\0";
    const char *target = NULL;
    int shouldSearchDirectory = 0, shouldSearchFiles = 0;
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-')
        {
            const char* ch = argv[i] + 1;
            while(*ch != '\0')
            {
                if(*ch == 'd') shouldSearchDirectory = 1;
                else if(*ch == 'f') shouldSearchFiles = 1;
                else
                {
                    Log(LOGL_ERROR, "discover: invalid flag %c specified\n", *ch);
                    return -1;
                }
                ch++;
            }
        } else if(argv[i][0] == '"')
        {
            if(target != NULL)
            {
                Log(LOGL_ERROR, "discover: two or more targets specified\n");
                return -1;
            }

            int n = strlen(argv[i]);
            if(argv[i][n - 1] != '"')
            {
                Log(LOGL_ERROR, "discover: invalid target dir\n");
                return -1;
            }
            target = argv[i];
        } else
        {
            if(searchDir[0] != '\0')
            {
                Log(LOGL_ERROR, "discover: two or more search dir specified\n");
                return -1;
            }
            strcpy(searchDir, argv[i]);
            // searchDir = argv[i];
        }
    }
    strcpy(searchDir, makePathAbsolute(searchDir));
    discoverDir(searchDir, target, !shouldSearchDirectory || shouldSearchFiles, !shouldSearchFiles || shouldSearchDirectory);
    return 0;
}
