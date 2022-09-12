#include "builtins.h"
#include "../core/process_list.h"

#include <termios.h>
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
            LogPError("fg: %s is not a valid index", argv[1]);
            return -1;
        }
        Log(LOGL_ERROR, "fg: %s is not a valid index\n", argv[1]);
        return -1;
    }

    pid_t pid = getProcessPID(index);
    if(pid == -1)
    {
        Log(LOGL_ERROR, "fg: %s is not a valid index\n", argv[1]);
        return -1;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &defTermiosAttr);
    tcsetpgrp(STDIN_FILENO, pid);
    if(kill(-pid, SIGCONT) == -1) // send the continue signal
    {
        tcsetpgrp(STDIN_FILENO, getpid());
        LogPError("fg");
        return -1;
    }

    const char* name = getProcessNameByPID(pid);
    removeProcess(pid);

    time_t s = time(NULL);
    waitpid(pid, &lastCommandStatus, WUNTRACED);
    time_t e = time(NULL);

    if(WIFSTOPPED(lastCommandStatus))
    {
        lastCommandStatus = 0;
        print("\n%s with pid = %d stopped\n", name, pid);
        addProcess(pid, name);
        setProcessStatus(pid, 1);
    }

    lastCommandTime = e - s;
    tcsetpgrp(STDIN_FILENO, getpid());
    tcsetattr(STDIN_FILENO, TCSANOW, &termiosAttr);

    return 0;
}
