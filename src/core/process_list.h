#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include "pipeline.h"
#include <sys/types.h>

typedef struct Process Process;
struct Process
{
    int isValid; // reuse same instead of free
    Process* next;
    PipelineJob job;
    pid_t pid;
    char name[100];
    int id;
    int status;
};

int addProcess(pid_t pid, const char name[]);
void removeProcess(pid_t pid);
void killAllProcesses();

const Process* getProcessListRoot();

const char* getProcessNameByPID(pid_t pid);

// returns pid of process at index `index` in the linked list
pid_t getProcessPID(int index);

// status := 1 -> Stopped, 0 -> Running
void setProcessStatus(pid_t pid, int status);

#endif // !PROCESS_LIST_H
