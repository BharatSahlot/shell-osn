#include "builtins.h"
#include "../utils.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

char curSearchPath[MAX_PATH_SIZE];
char curSearchAbsolutePath[MAX_PATH_SIZE];

void printFilteredPath(const char* path, const char* target)
{
    if(target != NULL)
    {
        int n1 = strlen(path), n2 = strlen(target);
        if(n1 < n2) return;

        for(int i = n1 - 1, j = n2 - 2; i >= 0 && j > 0; --i, --j)
        {
            if(path[i] != target[j]) return;
        }
    }
    static char temp[MAX_PATH_SIZE];
    strcpy(temp, curSearchPath);
    joinPaths(temp, path + strlen(curSearchAbsolutePath) + 1);

    int n = strlen(temp);
    if(temp[n - 1] == '/') temp[n - 1] = '\0';
    printf("%s\n", temp);
}

// absolute path of the directory
int discoverDir(const char* path, const char* target, int shouldSearchFiles, int shouldSearchDirectory)
{
    DIR* dir = opendir(path);
    if(dir == NULL)
    {
        LogPError("discover");
        return -1;
    }
    
    if(shouldSearchDirectory)
    {
        printFilteredPath(path, target);
    }

    struct stat st;

    errno = 0;
    struct dirent* item = readdir(dir);
    char tempPath[MAX_PATH_SIZE];
    while(item != NULL)
    {
        if(strcmp(item->d_name, "..") == 0 || strcmp(item->d_name, ".") == 0)
        {
            item = readdir(dir);
            continue;
        }

        strcpy(tempPath, path);
        joinPaths(tempPath, item->d_name);

        const char* abs = makePathAbsolute(tempPath);
        if(abs == NULL)
        {
            LogPError("discover %s", tempPath);
            continue;
        }
        strcpy(tempPath, abs);

        // dont follow symbolic links
        if(lstat(tempPath, &st) == -1)
        {
            item = readdir(dir);
            LogPError("discover");
            continue;
        }

        if(S_ISDIR(st.st_mode))
        {
            discoverDir(tempPath, target, shouldSearchFiles, shouldSearchDirectory);
            errno = 0;
        } else if(shouldSearchFiles)
        {
            printFilteredPath(tempPath, target);
        }
        item = readdir(dir);
    }
    free(item);
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
    int searchDirFound = 0;
    char searchDir[MAX_PATH_SIZE] = ".";
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
            if(searchDirFound)
            {
                Log(LOGL_ERROR, "discover: two or more search dir specified\n");
                return -1;
            }
            searchDirFound = 1;
            strcpy(searchDir, argv[i]);
        }
    }
    strcpy(curSearchPath, searchDir);
    strcpy(searchDir, makePathAbsolute(searchDir));
    strcpy(curSearchAbsolutePath, searchDir);
    discoverDir(searchDir, target, !shouldSearchDirectory || shouldSearchFiles, !shouldSearchFiles || shouldSearchDirectory);
    return 0;
}
