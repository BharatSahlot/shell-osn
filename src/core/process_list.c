#include "process_list.h"
#include "../globals.h"
#include "pipeline.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

Process* root = NULL;

const Process* getProcessListRoot()
{
    return root;
}

Process* next(Process* process)
{
    process = process->next;
    while(process != NULL)
    {
        if(process->isValid) break;
        process = process->next;
    }
    return process;
}

PipelineJob* getPipelineJobByPID(pid_t pid)
{
    Process* process = next(root);
    while(process != NULL)
    {
        if(process->isValid && process->job->pid == pid) return process->job;
        process = next(process);
    }
    return NULL;
}

PipelineJob* getPipelineJobByID(int id)
{
    Process* process = next(root);
    while(process != NULL)
    {
        if(process->isValid && process->id == id) return process->job;
        process = next(process);
    }
    return NULL;
}

Process* init_process(PipelineJob* job)
{
    Process* process = (Process*) malloc(sizeof(Process));
    process->next = NULL;
    process->id = -1;
    process->status = 0;
    process->isValid = 1;
    process->job = job;
    return process;
}

int addProcess(PipelineJob* job)
{
    if(root == NULL)
    {
        root = init_process(NULL);
    }

    Process* last = root;
    while(last->next != NULL)
    {
        if(!last->isValid)
        {
            last->job = job;
            last->isValid = 1;
            last->status = 0;
            return last->id;
        }
        last = last->next;
    }
    if(!last->isValid)
    {
        last->job = job;
        last->isValid = 1;
        last->status = 0;
        return last->id;
    }

    Process* process = init_process(job);
    process->id = last->id + 1;
    last->next = process;
    return process->id;
}

void removeProcess(pid_t pid)
{
    if(root == NULL) return;

    Process* process = next(root);
    while(process != NULL)
    {
        if(process->job->pid == pid)
        {
            process->isValid = 0;
            return;
        }
        process = next(process);
    }
}

void killAllProcesses()
{
    if(root == NULL) return;

    Process* process = root->next, *last = root;
    while(process != NULL)
    {
        if(process->isValid && process->job->pid > 0) kill(process->job->pid, SIGKILL);
        free(last);
        last = process;
        process = process->next;
    }
    if(last) free(last);
    root = NULL;
}

const char* getProcessNameByPID(pid_t pid)
{
    if(root == NULL) return NULL;
    
    Process* process = next(root);
    while(process != NULL)
    {
        if(process->job->pid == pid) return process->job->args[0];
        process = next(process);
    }
    return NULL;
}

pid_t getProcessPID(int index)
{
    if(root == NULL) return -1;

    Process* process = next(root);
    while(process != NULL)
    {
        if(process->id == index) return process->job->pid;
        process = next(process);
    }
    return -1;
}

void setProcessStatus(pid_t pid, int status)
{
    if(root == NULL) return;
    
    Process* process = next(root);
    while(process != NULL)
    {
        if(process->job->pid == pid)
        {
            process->status = status;
            break;
        }
        process = next(process);
    }
    return;
}
