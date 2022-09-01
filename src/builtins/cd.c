#include "builtins.h"
#include "../utils.h"

#include <string.h>
#include <unistd.h>

int prevPathSet = 0;
char prevPath[MAX_PATH_SIZE];

int cd(int argc, const char **argv)
{
    if(argc > 2)
    {
        Log(LOGL_ERROR, "cd: too many arguements\n");
        return -1;
    }

    if(strcmp(argv[1], "-") == 0)
    {
        if(!prevPathSet)
        {
            Log(LOGL_ERROR, "cd: prev path not set\n");
            return -1;
        }
        if(chdir(prevPath) == -1)
        {
            LogPError("cd");
            return -1;
        }
        strcpy(prevPath, currentPath);
        if(getWorkingDir() == NULL)
        {
            LogPError("cd");
            return -1;
        }
        return 0;
    }
    const char* path = argv[1];
    if(path[0] != '.') path = makePathAbsolute(argv[1]);

    if(chdir(path) == -1)
    {
        LogPError("cd");
        return -1;
    }
    strcpy(prevPath, currentPath);
    prevPathSet = 1;
    if(getWorkingDir() == NULL)
    {
        LogPError("cd");
        return -1;
    }
    return 0;
}
