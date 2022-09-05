#include "globals.h"
#include "builtins/builtins.h"
#include "core/execute.h"
#include "core/parse.h"
#include "core/prompt.h"
#include "core/history.h"
#include "logger.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>

int commandCount;
Command commandArr[10];
char currentPath[MAX_PATH_SIZE];
char home[MAX_PATH_SIZE];
int lastCommandStatus;
int lastCommandTime;
int bgProcessesRunning;
int shouldExitShell;

void zombie_handler(int sig, siginfo_t* info, void* ucontext)
{
    int status = 0;
    pid_t p = waitpid(info->si_pid, &status, WUNTRACED | WCONTINUED);
    if(p == -1)
    {
        // dont log error when foreground process exits
        if(errno != ECHILD) LogPError("zombie");
        return;
    }
    char name[50];
    lastCommandStatus = 0;
    const char* sta = "exited normally";
    switch (info->si_code) {
        case CLD_KILLED: lastCommandStatus = -1; sta = "was killed"; break;
        case CLD_DUMPED: lastCommandStatus = -1; sta = "exited abnormally"; break;
        case CLD_STOPPED: sta = "has stopped"; break;
        case CLD_CONTINUED: sta = "has continued"; break;
    }
    bgProcessesRunning--;

    if(tcgetpgrp(STDIN_FILENO) == getpid())
    {
        printf("\nProcess with pid = %d %s\n", info->si_pid, sta);
        render_prompt();
        fflush(stdout);
    }
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

    // ctrl-c should only exit child process not shell
    signal(SIGINT, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);

    if(initHistory() == -1) return -1;

    getWorkingDir();
    strcpy(home, currentPath);

    useBuiltins();

    char cmd[MAX_CMD_LENGTH];
    while(!shouldExitShell)
    {
        render_prompt();

        if(fgets(cmd, 249, stdin) == NULL)
        {
            LogPError("Error while reading stdin");
            return -1;
        }
        loadHistory();
        recordInHistory(cmd);
        parse(cmd);
        saveHistory();
    }
    return 0;
}
