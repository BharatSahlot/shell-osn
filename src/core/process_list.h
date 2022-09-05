#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include <sys/types.h>

typedef struct Process Process;
struct Process
{
    Process* next;
    pid_t pid;
    char name[100];
};

void addProcess(pid_t pid, const char name[]);
void removeProcess(pid_t pid);
void killAllProcesses();

const char* getProcessName(pid_t pid);

#endif // !PROCESS_LIST_H
