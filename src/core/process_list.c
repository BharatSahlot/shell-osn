#include "process_list.h"
#include "../globals.h"

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

Process* init_process(pid_t pid, const char name[])
{
    Process* process = (Process*) malloc(sizeof(Process));
    process->pid = pid;
    process->next = NULL;
    process->id = -1;
    process->status = 0;
    process->isValid = 1;
    memset(process->name, 0, sizeof(process->name));
    strcpy(process->name, name);
    return process;
}

int addProcess(pid_t pid, const char name[])
{
    if(root == NULL)
    {
        root = init_process(-1, "root");
    }

    Process* last = root;
    while(last->next != NULL)
    {
        if(!last->isValid)
        {
            last->pid = pid;
            strcpy(last->name, name);
            last->isValid = 1;
            last->status = 0;
            return last->id;
        }
        last = last->next;
    }

    Process* process = init_process(pid, name);
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
        if(process->pid == pid)
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
        if(process->isValid && process->pid > 0) kill(process->pid, SIGKILL);
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
        if(process->pid == pid) return process->name;
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
        if(process->id == index) return process->pid;
        process = next(process);
    }
    return -1;
}

void setProcessStatus(pid_t pid, int status)
{
    if(root == NULL) return;
    
    Process* process = root;
    while(process != NULL)
    {
        if(process->pid == pid)
        {
            process->status = status;
            break;
        }
        process = next(process);
    }
    return;
}
