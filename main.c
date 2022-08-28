#include <stdio.h>
#include <string.h>

#include "logger.h"
#include "utils.h"

char prompt[50];
char dir[50] = "~";
char cmd[250];

int main (int argc, char *argv[])
{
    sprintf(prompt, "%s@%s", getUserName(), getSystemName());

    while(1)
    {
        printf("<%s:%s>", prompt, dir);
        if(fgets(cmd, 249, stdin) == NULL)
        {
            Log(LOGL_ERROR, "Error while reading stdin");
            return -1;
        }

        int n = strlen(cmd);
        int inSingle = 0, inDouble = 0;

        char* st = cmd;
        for(int i = 0; i < n; i++)
        {
            if(cmd[i] == '\'') inSingle ^= 1;
            if(cmd[i] == '\"') inDouble ^= 1;

            if(inSingle || inDouble) continue;

            if(cmd[i] == '&')
            {
                cmd[i] = '\0';
                printf("bg: %s\n", st);
                st = cmd + i + 1;
            } else if(cmd[i] == ';')
            {
                cmd[i] = '\0';
                printf("fg: %s\n", st);
                st = cmd + i + 1;
            }
        }
        if(st < cmd + n) printf("fg: %s\n", st);
    }
    return 0;
}
