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

// int execute(int executeInBackground, const char *cmd, int argc, const char *argv[])
// {
//     if(!executeInBackground)
//     {
//         for(int i = 0; i < commandCount; i++)
//         {
//             if(strcmp(commandArr[i].cmd, cmd) == 0)
//             {
//                 lastCommandStatus = 0;
//                 int status = commandArr[i].func(argc, argv);
//                 lastCommandStatus = status;
//                 return status;
//             }
//         }
//     }
//
//     int term = STDIN_FILENO;
//     pid_t pid = fork();
//     if(pid == 0)
//     {
//         setpgid(pid, pid);
//         signal(SIGINT, SIG_DFL);
//         signal(SIGTTOU, SIG_DFL);
//         signal(SIGTTIN, SIG_DFL);
//         signal(SIGQUIT, SIG_DFL);
//         signal(SIGTSTP, SIG_DFL);
//         if(!executeInBackground)
//         {
//             tcsetpgrp(term, pid);
//         }
//         execvp(cmd, (char* const*)argv);
//         LogPError("\n%s", cmd); // child prints the error
//         exit(EXIT_FAILURE);
//     } else if(pid != -1)
//     {
//         tcsetattr(STDIN_FILENO, TCSANOW, &defTermiosAttr);
//         setpgid(pid, pid);
//         lastCommandStatus = 0;
//         if(!executeInBackground)
//         {
//             tcsetpgrp(term, pid);
//             time_t s = time(NULL);
//             waitpid(pid, &lastCommandStatus, WUNTRACED);
//             time_t e = time(NULL);
//
//             if(WIFSTOPPED(lastCommandStatus))
//             {
//                 lastCommandStatus = 0;
//                 print("\n%s with pid = %d stopped\n", cmd, pid);
//                 addProcess(pid, cmd);
//                 setProcessStatus(pid, 1);
//             }
//
//             lastCommandTime = e - s;
//             tcsetpgrp(term, getpid());
//             tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosAttr);
//         } else
//         {
//             int id = addProcess(pid, cmd);
//             tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosAttr);
//             print("[%d] %d\n", id, pid);
//         }
//         return lastCommandStatus;
//     }
//     lastCommandStatus = -1;
//     Log(LOGL_ERROR, "exec: %s not an inbuilt command\n", cmd);
//     return -1;
// }

int executeJob(int executeInBackground, PipelineJob* job)
{
    pid_t p = fork();
    if(p == 0)
    {
        setpgid(p, p);
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
            // close(job->fd[1]); // close for the child
        }

        if(job->fd[0] != -1) close(job->fd[0]);

        for(int i = 0; i < commandCount; i++)
        {
            if(strcmp(commandArr[i].cmd, job->args[0]) == 0)
            {
                lastCommandStatus = 0;
                int status = commandArr[i].func(job->argc, job->args);
                lastCommandStatus = status;
                if(lastCommandStatus) exit(EXIT_FAILURE);
                exit(EXIT_SUCCESS);
            }
        }

        execvp(job->args[0], (char* const*)job->args);
        LogPError("%s", job->args[0]); // child prints the error
        exit(EXIT_FAILURE);
    } else
    {
        job->pid = p;
        setpgid(p, p);
        lastCommandStatus = 0;

        if(!executeInBackground)
        {
            tcsetattr(STDIN_FILENO, TCSANOW, &defTermiosAttr);
            tcsetpgrp(STDIN_FILENO, p);
            time_t start = time(NULL);
            waitpid(p, &lastCommandStatus, WUNTRACED);
            time_t end = time(NULL);
            lastCommandTime += end - start;
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosAttr);
            tcsetpgrp(STDIN_FILENO, getpid());
        } else
        {
            int id = addProcess(job);
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosAttr);
            print("[%d] %d\n", id, job->pid);
        }
    }
    return lastCommandStatus;
}

int executePipeline(int executeInBackground, PipelineJob* pipelineJob, int elapsed)
{
    if(!executeInBackground && pipelineJob->next == NULL)
    {
        lastCommandTime = elapsed;
        for(int i = 0; i < commandCount; i++)
        {
            if(strcmp(commandArr[i].cmd, pipelineJob->args[0]) == 0)
            {
                lastCommandStatus = 0;
                int status = commandArr[i].func(pipelineJob->argc, pipelineJob->args);
                lastCommandStatus = status;
                return lastCommandStatus;
            }
        }
    }

    if(executeInBackground && pipelineJob->next != NULL)
    {
        Log(LOGL_ERROR, "execute error: pipeline jobs cannot be piped\n");
        return -1;
    }

    int stdin = dup(STDIN_FILENO);
    int stdout = dup(STDOUT_FILENO);

    lastCommandTime = elapsed;
    lastCommandStatus = 0;
    PipelineJob* job = pipelineJob;
    while(job != NULL)
    {
        if(job->next != NULL)
        {
            if(pipe(job->fd) < 0)
            {
                cleanPipeline(pipelineJob);
                dup2(stdin, STDIN_FILENO);
                dup2(stdout, STDOUT_FILENO);
                LogPError("pipe error");
                return -1;
            }
        }
        if(job->prev != NULL) job->in = job->prev->fd[0];

        lastCommandStatus = 0;
        executeJob(executeInBackground, job);

        if(WIFSTOPPED(lastCommandStatus))
        {
            print("\n%s with pid = %d stopped\n", job->args[0], job->pid);
            addProcess(job);
            setProcessStatus(job->pid, 1);
            return 0;
        } else if(WIFSIGNALED(lastCommandStatus))
        {
            cleanPipeline(pipelineJob);
            return -1;
        }

        if(job->fd[1] != -1)
        {
            close(job->fd[1]);
            job->fd[1] = -1;
        }
        dup2(stdout, STDOUT_FILENO);
        job = job->next;
    }
    dup2(stdin, STDIN_FILENO);
    cleanPipeline(pipelineJob);
    return 0;
}
