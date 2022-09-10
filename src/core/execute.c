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

    int term = STDIN_FILENO;
    pid_t ppid = getpid();
    pid_t pid = fork();
    if(pid == 0)
    {
        setpgid(pid, pid);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        if(!executeInBackground)
        {
            tcsetpgrp(term, pid);
        }
        execvp(cmd, (char* const*)argv);
        LogPError("%s", cmd); // child prints the error
        lastCommandStatus = -1;
        tcsetpgrp(term, ppid);
        exit(-1);
    } else if(pid != -1)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &defTermiosAttr);
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
            tcsetattr(STDIN_FILENO, TCSANOW, &termiosAttr);
        } else
        {
            addProcess(pid, cmd);
            bgProcessesRunning++;
            tcsetattr(STDIN_FILENO, TCSANOW, &termiosAttr);
            printf("[%d] %d\n", bgProcessesRunning, pid);
        }
        return lastCommandStatus;
    }
    lastCommandStatus = -1;
    Log(LOGL_ERROR, "exec: %s not an inbuilt command\n", cmd);
    return -1;
}
