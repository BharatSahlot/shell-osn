#include "process_list.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>

Process* root = NULL;

Process* init_process(pid_t pid, const char name[])
{
    Process* process = (Process*) malloc(sizeof(Process));
    process->pid = pid;
    process->next = NULL;
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
    last->next = process;
}

void removeProcess(pid_t pid)
{
    if(root == NULL) return;

    Process* process = root, *last = NULL;
    while(process->next != NULL)
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

    Process* process = root, *last = NULL;
    while(process->next != NULL)
    {
        kill(process->pid, SIGKILL);
        if(last) free(last);
        last = process;
        process = process->next;
    }
    if(last) free(last);
}

const char* getProcessName(pid_t pid)
{
    if(root == NULL) return NULL;
    
    Process* process = root;
    while(process->next != NULL)
    {
        if(process->pid == pid) return process->name;
        process = process->next;
    }
    return NULL;
}
