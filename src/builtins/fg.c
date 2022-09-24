#include "builtins.h"
#include "../core/process_list.h"
#include "../core/execute.h"

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

    PipelineJob* pipelineJob = getPipelineJobByPID(pid);
    removeProcess(pid);

    PipelineJob* job = pipelineJob;

    // while(job != NULL)
    // {
    //     print("job [%s] with pid [%d]\n", job->args[0], job->pid);
    //     job = job->next;
    // }
    // job = pipelineJob;

    tcsetattr(STDIN_FILENO, TCSANOW, &defTermiosAttr);
    tcsetpgrp(STDIN_FILENO, pid);
    if(kill(-job->pid, SIGCONT) == -1)
    {
        cleanPipeline(pipelineJob);
        tcsetpgrp(STDIN_FILENO, getpid());
        LogPError("fg");
        return -1;
    }

    time_t start = time(NULL);
    if(waitpid(job->pid, &lastCommandStatus, WUNTRACED) < 0)
    {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosAttr);
        tcsetpgrp(STDIN_FILENO, getpid());
        LogPError("waitpid");
        return -1;
    }
    time_t end = time(NULL);
    lastCommandTime += end - start;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosAttr);
    tcsetpgrp(STDIN_FILENO, getpid());

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

    job = job->next;

    if(job == NULL) return lastCommandStatus;

    return executePipeline(0, job, lastCommandTime);
}
