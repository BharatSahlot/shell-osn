#include "builtins.h"
#include "../core/process_list.h"

#include <errno.h>
#include <signal.h>
#include <stdlib.h>

int bg(int argc, const char* argv[])
{
    if(argc < 2)
    {
        Log(LOGL_ERROR, "bg: too less arguements\n");
        return -1;
    }
    if(argc > 2)
    {
        Log(LOGL_ERROR, "bg: too many arguements\n");
        return -1;
    }

    char ch;
    char* endptr = &ch;
    errno = 0;
    long index = strtol(argv[1], &endptr, 10);
    if(*endptr != '\0' || errno != 0)
    {
        if(errno != 0)
        {
            LogPError("bg: %s is not a valid index", argv[1]);
            return -1;
        }
        Log(LOGL_ERROR, "bg: %s is not a valid index\n", argv[1]);
        return -1;
    }

    pid_t pid = getProcessPID(index);
    if(pid == -1)
    {
        Log(LOGL_ERROR, "bg: %s is not a valid index\n", argv[1]);
        return -1;
    }

    if(kill(-pid, SIGCONT) == -1)
    {
        LogPError("bg");
        return -1;
    }
    return 0;
}
