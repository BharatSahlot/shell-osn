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
#include <signal.h>
#include <sys/wait.h>
#include <wait.h>

int commandCount;
Command commandArr[10];
char currentPath[MAX_PATH_SIZE];
char home[MAX_PATH_SIZE];
int lastCommandStatus;
int lastCommandTime;
int bgProcessesRunning;

const char* historyFile = "/tmp/batak-history.txt";

void zombie_handler(int sig, siginfo_t* info, void* ucontext)
{
    int status = 0;
    pid_t p = waitpid(info->si_pid, &status, WUNTRACED | WCONTINUED);
    if(p == -1)
    {
        LogPError("zombie");
        return;
    }
    char name[50];
    const char* sta = "exited normally";
    switch (info->si_code) {
        case CLD_KILLED: sta = "was killed"; break;
        case CLD_DUMPED: sta = "exited abnormally"; break;
        case CLD_STOPPED: sta = "has stopped"; break;
        case CLD_CONTINUED: sta = "has continued"; break;
    }
    bgProcessesRunning--;
    printf("\nProcess with pid = %d %s\n", info->si_pid, sta);
    render_prompt();
    fflush(stdout);
    fflush(stdin);
}

int main (int argc, char *argv[])
{
    struct sigaction st;
    st.sa_sigaction = zombie_handler;
    st.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&st.sa_mask);
    if(sigaction(SIGCHLD, &st, NULL) == -1)
    {
        LogPError("zombie");
        return -1;
    }

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
