#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "logger.h"
#include "utils.h"
#include "cmds/cmds.h"

char prompt[50];
char cmd[250];
char path[50] = "~";
char absolute_path[50];

typedef struct
{
    const char* cmd;
    void (*func) (char* args);
} Command;

Command commands[3];

void ExecuteCommand(char* cmd)
{
    char* p = strtok(cmd, " \n");
    if(p == NULL) return;
    for(int i = 0; i < 3; i++)
    {
        if(strcmp(p, commands[i].cmd) == 0)
        {
            // Log(LOGL_INFO, "%s is an inbuilt command", p);
            commands[i].func(p + strlen(p) + 1);
            return;
        }
    }
    // find executable
    Log(LOGL_ERROR, "%s not an inbuilt command\n", p);
}

int main (int argc, char *argv[])
{
    getcwd(absolute_path, 50);

    commands[0].cmd = "cd";
    commands[0].func = cd;

    commands[1].cmd = "echo";
    commands[1].func = echo;

    commands[2].cmd = "pwd";
    commands[2].func = pwd;

    sprintf(prompt, "%s@%s", getUserName(), getSystemName());

    while(1)
    {
        printf("\033[36m<%s:%s> \033[0m", prompt, path);
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
    return 0;
}
