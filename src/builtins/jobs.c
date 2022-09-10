#include "builtins.h"
#include "../core/process_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const Process* processes[1000];

int processCmp(const void* _a, const void* _b)
{
    const Process* a = (const Process*)_a;
    const Process* b = (const Process*)_b;
    return strcmp(a->name, b->name);
}

int jobs(int argc, const char **argv)
{
    int flags[2] = { 0, 0 };
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            Log(LOGL_ERROR, "jobs: invalid arguement %s\n", argv[i]);
            return -1;
        }

        const char* ch = argv[i] + 1;
        while(*ch != '\0')
        {
            if(*ch == 'r') flags[0] = 1;
            else if(*ch == 's') flags[1] = 1;
            else
            {
                Log(LOGL_ERROR, "jobs: invalid flag %c\n", *ch);
                return -1;
            }
            ch++;
        }
    }

    int showRunningJobs = flags[0] || !flags[1];
    int showStoppedJobs = flags[1] || !flags[0];

    int count = 0;
    const Process* root = getProcessListRoot();
    if(root != NULL) root = root->next;
    while(root != NULL)
    {
        if(!root->isValid || (!showRunningJobs && root->status == 0) ||
            (!showStoppedJobs && root->status == 1))
        {
            root = root->next;
            continue;
        }
        processes[count++] = root;
        root = root->next;
    }
    qsort(processes, count, sizeof(const Process*), processCmp);

    for(int i = 0; i < count; i++)
    {
        const char* status = processes[i]->status == 1 ? "Stopped" : "Running";
        print("[%d] %s %s [%d]\n", processes[i]->id, status, processes[i]->name, processes[i]->pid);
    }
    return 0;
}
