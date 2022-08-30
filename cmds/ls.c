#include "cmds.h"
#include "../logger.h"

#include <string.h>

void ls(int argc, char **argv)
{
    int displayHiddenFiles = 0;
    int displayExtraInfo = 0;

    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-') continue;
        if(strcmp(argv[i], "-a") == 0) displayHiddenFiles = 1;
        else if(strcmp(argv[i], "-l") == 0) displayExtraInfo = 1;
        else if(strcmp(argv[i], "-al") == 0 || strcmp(argv[i], "-la") == 0) displayExtraInfo = displayHiddenFiles = 1;
        else
        {
            Log(LOGL_ERROR, "ls: Unsupported arguement %s\n", argv[i]);
            return;
        }
    }
}
