#include "execute.h"
#include "../globals.h"

#include <string.h>

int execute(int executeInBackground, const char *cmd, int argc, const char **argv)
{
    if(executeInBackground)
    {
        Log(LOGL_ERROR, "exec: execute in background not implemented\n");
        return -1;
    }
    for(int i = 0; i < commandCount; i++)
    {
        if(strcmp(commandArr[i].cmd, cmd) == 0)
        {
            int status = commandArr[i].func(argc, argv);
            lastCommandStatus = status;
            return status;
        }
    }
    lastCommandStatus = -1;
    Log(LOGL_ERROR, "exec: %s not an inbuilt command\n", cmd);
    return -1;
}
