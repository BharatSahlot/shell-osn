#include "builtins.h"
#include "../core/process_list.h"

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>

int fg(int argc, const char* argv[])
{
    if(argc > 2)
    {
        Log(LOGL_ERROR, "fg: too many arguements\n");
        return -1;
    }
    if(argc < 2)
    {
        Log(LOGL_ERROR, "fg: too few arguements\n");
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
            LogPError("sig: %s is not a valid index", argv[1]);
            return -1;
        }
        Log(LOGL_ERROR, "sig: %s is not a valid index\n", argv[1]);
        return -1;
    }

    pid_t pid = getProcessPID(index);
    if(pid == -1)
    {
        Log(LOGL_ERROR, "sig: %s is not a valid index\n", argv[1]);
        return -1;
    }

    tcsetpgrp(STDIN_FILENO, pid);
    kill(-pid, SIGCONT); // send the continue signal
    time_t s = time(NULL);
    waitpid(pid, &lastCommandStatus, 0);
    time_t e = time(NULL);
    lastCommandTime = e - s;
    removeProcess(pid);
    tcsetpgrp(STDIN_FILENO, getpid());

    return 0;
}
