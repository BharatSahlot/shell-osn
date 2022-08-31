#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"
#include "utils.h"
#include "cmds/cmds.h"

char prompt[50];
char cmd[250];
char path[250] = "~";
char home_path[250];

char* argsBuf[10];

typedef struct
{
    const char* cmd;
    int shouldTokenize;
    void (*func) (int argc, char* argv[]);
} Command;

Command commands[4];

void ExecuteCommand(char* cmd)
{
    char* p = strtok(cmd, " \t\n");
    if(p == NULL) return;
    for(int i = 0; i < 4; i++)
    {
        if(strcmp(p, commands[i].cmd) == 0)
        {
            int argc = 0;
            if(commands[i].shouldTokenize)
            {
                while(p != NULL)
                {
                    strcpy(argsBuf[argc++], p);
                    p = strtok(NULL, " \t\n");
                }
            } else
            {
                strcpy(argsBuf[argc++], p);
                strcpy(argsBuf[argc++], p + strlen(p) + 1);
            }
            commands[i].func(argc, argsBuf);
            return;
        }
    }
    // find executable
    Log(LOGL_ERROR, "%s not an inbuilt command\n", p);
}

int main (int argc, char *argv[])
{
    for(int i = 0; i < 10; i++) argsBuf[i] = (char*) malloc(50 * sizeof(char));

    getcwd(home_path, 250);

    commands[0].cmd = "cd";
    commands[0].func = cd;
    commands[0].shouldTokenize = 1;

    commands[1].cmd = "echo";
    commands[1].func = echo;
    commands[1].shouldTokenize = 1;

    commands[2].cmd = "pwd";
    commands[2].func = pwd;
    commands[2].shouldTokenize = 1;

    commands[3].cmd = "ls";
    commands[3].func = ls;
    commands[3].shouldTokenize = 1;

    const char* sysName = getSystemName();
    if(sysName == NULL)
    {
        LogPError("Prompt");
        sysName = "Linux";
    }
    sprintf(prompt, "%s@%s", getUserName(), sysName);

    while(1)
    {
        printf("\033[90m<%s:\033[1;33m%s\033[90m> \033[0m", prompt, path);
        if(fgets(cmd, 249, stdin) == NULL)
        {
            LogPError("Error while reading stdin");
            return -1;
        }

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
                printf("bg: %s\n", st);
                st = cmd + i + 1;
            } else if(cmd[i] == ';')
            {
                cmd[i] = '\0';
                // printf("fg: %s\n", st);
                ExecuteCommand(st);
                st = cmd + i + 1;
            }
        }
        if(st < cmd + n)
        {
            ExecuteCommand(st);
            // printf("fg: %s\n", st);
        }
    }

    for(int i = 0; i < 10; i++) free(argsBuf[i]);
    return 0;
}
