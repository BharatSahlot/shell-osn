#include "core/autocomplete.h"
#include "globals.h"
#include "builtins/builtins.h"
#include "core/execute.h"
#include "core/parse.h"
#include "core/prompt.h"
#include "core/history.h"
#include "core/process_list.h"
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
#include <assert.h>
#include <termios.h>

int commandCount;
Command commandArr[10];
char currentPath[MAX_PATH_SIZE];
char home[MAX_PATH_SIZE];
int lastCommandStatus;
long int lastCommandTime;
int bgProcessesRunning;
int shouldExitShell;
struct termios termiosAttr, defTermiosAttr;

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
    lastCommandStatus = 0;
    const char* sta = "exited normally";
    switch (info->si_code) {
        case CLD_KILLED: lastCommandStatus = -1; sta = "was killed"; break;
        case CLD_DUMPED: lastCommandStatus = -1; sta = "exited abnormally"; break;
        case CLD_STOPPED: sta = "has stopped"; break;
        case CLD_CONTINUED: sta = "has continued"; break;
    }
    bgProcessesRunning--;
    const char* processName = getProcessName(p);
    if(processName != NULL)
    {
        printf("\n%s with pid = %d %s\n", processName, info->si_pid, sta);
        removeProcess(p);
    }
    if(tcgetpgrp(STDIN_FILENO) == getpid())
    {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &termiosAttr);
        render_prompt();
        fflush(stdout);
    }
}

int main ()
{
    setpgrp();
    tcsetpgrp(STDIN_FILENO, getpid());
    tcgetattr(STDIN_FILENO, &defTermiosAttr);
    tcgetattr(STDIN_FILENO, &termiosAttr);
    termiosAttr.c_lflag &= ~( ICANON | ECHO );
    termiosAttr.c_cc[VMIN] = 1;
    termiosAttr.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &termiosAttr);

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
    // signal(SIGINT, SIG_IGN);
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
        fflush(stdout);
        int cmdLength = 0;
        while(1)
        {
            read(STDIN_FILENO, &cmd[cmdLength], 1);
            char ch = cmd[cmdLength];
            switch (ch) {
                case '\x1b': // arrow keys
                {
                    char buf[3];
                    buf[2] = 0;
                    read(STDIN_FILENO, buf, 2);
                    continue;
                }
                case '\t':
                {
                    if(cmdLength == 0) continue;
                    autocomplete(cmdLength, cmd);
                    printf("\n");
                    render_prompt();
                    for(int i = 0; i < cmdLength; i++)
                    {
                        printf("%c", cmd[i]);
                    }
                    fflush(stdout);
                    continue;
                }
                case '\177':
                {
                    if(cmdLength > 0)
                    {
                        cmdLength--;
                        printf("\b \b");
                        fflush(stdout);
                    }
                    continue;
                }
                default:
                {
                    printf("%c", cmd[cmdLength]);
                    fflush(stdout);
                }
            }
            if(cmd[cmdLength] == '\n')
            {
                cmd[cmdLength] = '\0';
                break;
            }
            cmdLength++;
        }
        loadHistory();
        recordInHistory(cmd);
        parse(cmd);
        saveHistory();
    }
    signal(SIGCHLD, SIG_DFL);
    killAllProcesses();
    return 0;
}
