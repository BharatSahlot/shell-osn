#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include "pipeline.h"
#include <sys/types.h>

typedef struct Process Process;
struct Process
{
    int isValid; // reuse same instead of free
    Process* next;
    PipelineJob* job;
    int id;
    int status;
};

int addProcess(PipelineJob* job);
void removeProcess(pid_t pid);
void killAllProcesses();

const Process* getProcessListRoot();

const char* getProcessNameByPID(pid_t pid);

PipelineJob* getPipelineJobByPID(pid_t pid);
PipelineJob* getPipelineJobByID(int id);

// returns pid of process at index `index` in the linked list
pid_t getProcessPID(int index);

// status := 1 -> Stopped, 0 -> Running
void setProcessStatus(pid_t pid, int status);

#endif // !PROCESS_LIST_H
