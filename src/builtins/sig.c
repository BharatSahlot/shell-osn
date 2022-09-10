#include "builtins.h"
#include "../core/process_list.h"

#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int sig(int argc, const char **argv)
{
    if(argc > 3)
    {
        Log(LOGL_ERROR, "sig: too many arguements given\n");
        return -1;
    }

    char* endptr = malloc(sizeof(char));
    errno = 0;
    long index = strtol(argv[1], &endptr, 10);
    if(*endptr != '\0' || errno != 0)
    {
        if(errno != 0)
        {
            LogPError("sig: %s is not a valid pid", argv[1]);
            return -1;
        }
        Log(LOGL_ERROR, "sig: %s is not a valid pid.\n", argv[1]);
        return -1;
    }
    pid_t pid = getProcessPID(index);
    if(pid == -1)
    {
        Log(LOGL_ERROR, "sig: process with index %d not found\n", index);
        return -1;
    }

    int sign = strtol(argv[2], &endptr, 10);
    if(*endptr != '\0' || errno != 0)
    {
        if(errno != 0)
        {
            LogPError("sig: %s is not a valid signal", argv[2]);
            return -1;
        }
        Log(LOGL_ERROR, "sig: %s is not a valid signal.\n", argv[2]);
        return -1;
    }
    if(kill(pid, sign) == -1)
    {
        LogPError("sig");
        return -1;
    }
    return 0;
}
