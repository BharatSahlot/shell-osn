#include "parse.h"
#include "execute.h"
#include "pipeline.h"
#include "process_list.h"

#include <string.h>
#include <stdio.h>

// int argc = 0;
// const char* argsBuf[MAX_ARGS];
//
// const char** parseArgs(char* cmd)
// {
//     argc = 0;
//     char* p = strtok(cmd, " \t\n");
//     while(p != NULL)
//     {
//         argsBuf[argc++] = p;
//         p = strtok(NULL, " \t\n");
//     }
//     argsBuf[argc] = NULL; // null terminate args list, usefull for sys calls like execve
//     return argsBuf;
// }
//
void parse(char *cmd)
{
    int n = strlen(cmd);
    int inSingle = 0, inDouble = 0;

    char* st = cmd;
    for(int i = 0; i < n; i++)
    {
        if(!inDouble && cmd[i] == '\'') inSingle ^= 1;
        if(!inSingle && cmd[i] == '\"') inDouble ^= 1;

        if(inSingle || inDouble) continue;

        if(cmd[i] == '&')
        {
            cmd[i] = '\0';
            PipelineJob* rootJob = parsePipeline(st);
            if(rootJob == NULL)
            {
                st = cmd + i + 1;
                continue;
            }
            executePipeline(1, rootJob);
            st = cmd + i + 1;
        } else if(cmd[i] == ';')
        {
            cmd[i] = '\0';
            PipelineJob* rootJob = parsePipeline(st);
            if(rootJob == NULL)
            {
                st = cmd + i + 1;
                continue;
            }
            executePipeline(0, rootJob);
            st = cmd + i + 1;
        }
    }
    if(st < cmd + n)
    {
        PipelineJob* rootJob = parsePipeline(st);
        if(rootJob != NULL) executePipeline(0, rootJob);
    }
}
