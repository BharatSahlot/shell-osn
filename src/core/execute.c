#include "execute.h"
#include "../globals.h"
#include "process_list.h"

#include <stdlib.h>
#include <termios.h>
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

    struct termios t;
    if(tcgetattr(STDIN_FILENO, &t) == -1)
    {
        LogPError("exec");
        lastCommandStatus = -1;
        return -1;
    }

    int term = STDIN_FILENO;
    pid_t ppid = getpid();
    pid_t pid = fork();
    if(pid == 0)
    {
        setpgid(pid, pid);
        if(!executeInBackground)
        {
            tcsetpgrp(term, pid);
        }
        signal(SIGINT, SIG_DFL);
        if(execve(cmd, (char* const*)argv, NULL) == -1)
        {
            LogPError("%s", cmd);
            lastCommandStatus = -1;
            tcsetpgrp(term, ppid);
            exit(-1);
            return -1;
        }
        lastCommandStatus = 0;
        return 0;
    } else if(pid != -1)
    {
        setpgid(pid, pid);
        lastCommandStatus = 0;
        if(!executeInBackground)
        {
            tcsetpgrp(term, pid);
            time_t s = time(NULL);
            waitpid(pid, &lastCommandStatus, 0);
            time_t e = time(NULL);
            lastCommandTime = e - s;
            tcsetpgrp(term, getpid());
        } else
        {
            addProcess(pid, cmd);
            bgProcessesRunning++;
            printf("[%d] %d\n", bgProcessesRunning, pid);
        }
        tcsetattr(term, TCSADRAIN, &t);
        return lastCommandStatus;
    }
    lastCommandStatus = -1;
    Log(LOGL_ERROR, "exec: %s not an inbuilt command\n", cmd);
    return -1;
}
