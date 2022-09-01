#include "parse.h"
#include "execute.h"

#include <string.h>
#include <stdio.h>

int argc = 0;
const char* argsBuf[MAX_ARGS];

const char** parseArgs(char* cmd)
{
    argc = 0;
    char* p = strtok(cmd, " \t\n");
    while(p != NULL)
    {
        argsBuf[argc++] = p;
        p = strtok(NULL, " \t\n");
    }
    return argsBuf;
}

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
            // printf("bg: %s\n", st);
            parseArgs(st);
            execute(1, argsBuf[0], argc, argsBuf);
            st = cmd + i + 1;
        } else if(cmd[i] == ';')
        {
            cmd[i] = '\0';
            // printf("fg: %s\n", st);
            // ExecuteCommand(st);
            parseArgs(st);
            execute(0, argsBuf[0], argc, argsBuf);
            st = cmd + i + 1;
        }
    }
    if(st < cmd + n)
    {
        parseArgs(st);
        execute(0, argsBuf[0], argc, argsBuf);
        // ExecuteCommand(st);
        // printf("fg: %s\n", st);
    }
}