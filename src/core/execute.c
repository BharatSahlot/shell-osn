#include "execute.h"
#include "../globals.h"
#include "io.h"
#include "pipeline.h"
#include "process_list.h"

#include <stdlib.h>
#include <sys/types.h>
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
    pid_t pid = fork();
    if(pid == 0)
    {
        setpgid(pid, pid);
        signal(SIGINT, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        if(!executeInBackground)
        {
            tcsetpgrp(term, pid);
        }
        execvp(cmd, (char* const*)argv);
        LogPError("\n%s", cmd); // child prints the error
        exit(EXIT_FAILURE);
    } else if(pid != -1)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &defTermiosAttr);
        setpgid(pid, pid);
        lastCommandStatus = 0;
        if(!executeInBackground)
        {
            tcsetpgrp(term, pid);
            time_t s = time(NULL);
            waitpid(pid, &lastCommandStatus, WUNTRACED);
            time_t e = time(NULL);

            if(WIFSTOPPED(lastCommandStatus))
            {
                lastCommandStatus = 0;
                print("\n%s with pid = %d stopped\n", cmd, pid);
                addProcess(pid, cmd);
                setProcessStatus(pid, 1);
            }

            lastCommandTime = e - s;
            tcsetpgrp(term, getpid());
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosAttr);
        } else
        {
            int id = addProcess(pid, cmd);
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosAttr);
            print("[%d] %d\n", id, pid);
        }
        return lastCommandStatus;
    }
    lastCommandStatus = -1;
    Log(LOGL_ERROR, "exec: %s not an inbuilt command\n", cmd);
    return -1;
}

int executeJob(int executeInBackground, PipelineJob* job)
{
    pid_t p = fork();
    if(p == 0)
    {
        setpgid(p, p);
        job->pid = getpid();
        signal(SIGINT, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        if(!executeInBackground)
        {
            tcsetpgrp(STDIN_FILENO, getpid());
        }

        if(job->in != -1)
        {
            dup2(job->in, STDIN_FILENO);
            close(job->in); // close for the child
        }

        if(job->fd[1] != -1)
        {
            dup2(job->fd[1], STDOUT_FILENO);
            close(job->fd[1]); // close for the child
        }

        if(job->fd[0] != -1) close(job->fd[0]);

        execvp(job->args[0], (char* const*)job->args);
        LogPError("\n%s", job->args[0]); // child prints the error
        exit(EXIT_FAILURE);
    } else
    {
        setpgid(p, p);
        tcsetpgrp(STDIN_FILENO, p);
        tcsetattr(STDIN_FILENO, TCSANOW, &defTermiosAttr);
        lastCommandStatus = 0;

        if(!executeInBackground)
        {
            time_t start = time(NULL);
            waitpid(p, &lastCommandStatus, WUNTRACED);
            time_t end = time(NULL);

            lastCommandTime = end - start;
        } else
        {
        }
        tcsetpgrp(STDIN_FILENO, getpid());
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosAttr);
    }
    return lastCommandStatus;
}

int executePipeline(int executeInBackground, PipelineJob* pipelineJob)
{
    if(executeInBackground && pipelineJob->next != NULL)
    {
        Log(LOGL_ERROR, "execute error: pipeline jobs cannot be piped\n");
        return -1;
    }

    int stdin = dup(STDIN_FILENO);
    int stdout = dup(STDOUT_FILENO);

    PipelineJob* job = pipelineJob;
    while(job != NULL)
    {
        if(job->next != NULL)
        {
            if(pipe(job->fd) < 0)
            {
                // clean job file descriptors
                LogPError("pipe error");
                return -1;
            }
        }
        if(job->prev != NULL) job->in = job->prev->fd[0];
        executeJob(executeInBackground, job);

        if(job->fd[1] != -1)
        {
            close(job->fd[1]);
            job->fd[1] = -1;
            dup2(stdout, STDOUT_FILENO);
        }

        job = job->next;
    }
    dup2(stdin, STDIN_FILENO);
    return 0;
}
