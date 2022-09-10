#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include <sys/types.h>

typedef struct Process Process;
struct Process
{
    Process* next;
    pid_t pid;
    char name[100];
    int id;
    int status;
};

void addProcess(pid_t pid, const char name[]);
void removeProcess(pid_t pid);
void killAllProcesses();

const Process* getProcessListRoot();

const char* getProcessName(pid_t pid);

// status := 1 -> Stopped, 0 -> Running
void setProcessStatus(pid_t pid, int status);

#endif // !PROCESS_LIST_H
