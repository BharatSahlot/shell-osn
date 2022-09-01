#include "globals.h"
#include "builtins/builtins.h"
#include "core/execute.h"
#include "core/parse.h"
#include "core/prompt.h"
#include "logger.h"

#include <stdio.h>

int commandCount;
Command commandArr[10];
char currentPath[MAX_PATH_SIZE];
char home[MAX_PATH_SIZE];
int lastCommandStatus;
long long lastCommandTime;

int main (int argc, char *argv[])
{
    char cmd[MAX_CMD_LENGTH];
    while(1)
    {
        render_prompt();

        if(fgets(cmd, 249, stdin) == NULL)
        {
            LogPError("Error while reading stdin");
            return -1;
        }
        parse(cmd);
    }
    return 0;
}
