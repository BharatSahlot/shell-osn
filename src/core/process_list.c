#include "process_list.h"
#include "../globals.h"

#include <signal.h>
#include <stdlib.h>
#include <string.h>

Process* root = NULL;

const Process* getProcessListRoot()
{
    return root;
}

Process* init_process(pid_t pid, const char name[])
{
    Process* process = (Process*) malloc(sizeof(Process));
    process->pid = pid;
    process->next = NULL;
    process->id = -1;
    process->status = 0;
    memset(process->name, 0, sizeof(process->name));
    strcpy(process->name, name);
    return process;
}

void addProcess(pid_t pid, const char name[])
{
    if(root == NULL)
    {
        root = init_process(-1, "root");
    }

    Process* last = root;
    while(last->next != NULL) last = last->next;
    Process* process = init_process(pid, name);
    process->id = bgProcessesRunning;
    last->next = process;
}

void removeProcess(pid_t pid)
{
    if(root == NULL) return;

    Process* process = root, *last = NULL;
    while(process != NULL)
    {
        if(process->pid == pid)
        {
            last->next = process->next;
            free(process);
            return;
        }
        last = process;
        process = process->next;
    }
}

void killAllProcesses()
{
    if(root == NULL) return;

    Process* process = root->next, *last = root;
    while(process != NULL)
    {
        if(process->pid > 0) kill(process->pid, SIGKILL);
        free(last);
        last = process;
        process = process->next;
    }
    if(last) free(last);
    root = NULL;
}

const char* getProcessName(pid_t pid)
{
    if(root == NULL) return NULL;
    
    Process* process = root;
    while(process != NULL)
    {
        if(process->pid == pid) return process->name;
        process = process->next;
    }
    return NULL;
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
        process = process->next;
    }
    return;
}
