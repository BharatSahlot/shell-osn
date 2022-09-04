#include "execute.h"
#include "../globals.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

int execute(int executeInBackground, const char *cmd, int argc, const char *argv[])
{
    if(!executeInBackground)
    {
        for(int i = 0; i < commandCount; i++)
        {
            if(strcmp(commandArr[i].cmd, cmd) == 0)
            {
                lastCommandStatus = 0;
                int status = commandArr[i].func(argc, argv);
                lastCommandStatus = status;
                return status;
            }
        }
    }

    pid_t pid = fork();
    if(pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        if(execve(cmd, (char* const*)argv, NULL) == -1)
        {
            LogPError("%s", cmd);
            lastCommandStatus = -1;
            return -1;
        }
        lastCommandStatus = 0;
        return 0;
    } else if(pid != -1)
    {
        lastCommandStatus = 0;
        if(!executeInBackground)
        {
            time_t s = time(NULL);
            wait(&lastCommandStatus);
            time_t e = time(NULL);
            lastCommandTime = e - s;
        } else
        {
            bgProcessesRunning++;
            printf("[%d] %d\n", bgProcessesRunning, pid);
        }
        return lastCommandStatus;
    }
    lastCommandStatus = -1;
    Log(LOGL_ERROR, "exec: %s not an inbuilt command\n", cmd);
    return -1;
}
