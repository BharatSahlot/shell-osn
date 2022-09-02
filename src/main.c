#include "globals.h"
#include "builtins/builtins.h"
#include "core/execute.h"
#include "core/parse.h"
#include "core/prompt.h"
#include "core/history.h"
#include "logger.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

int commandCount;
Command commandArr[10];
char currentPath[MAX_PATH_SIZE];
char home[MAX_PATH_SIZE];
int lastCommandStatus;
int lastCommandTime;

const char* historyFile = "/tmp/batak-history.txt";

int main (int argc, char *argv[])
{
    loadHistory(historyFile);

    getWorkingDir();
    strcpy(home, currentPath);

    useBuiltins();

    char cmd[MAX_CMD_LENGTH];
    while(1)
    {
        render_prompt();

        if(fgets(cmd, 249, stdin) == NULL)
        {
            LogPError("Error while reading stdin");
            return -1;
        }
        loadHistory(historyFile);
        recordInHistory(cmd);
        parse(cmd);
        saveHistory(historyFile);
    }
    return 0;
}
