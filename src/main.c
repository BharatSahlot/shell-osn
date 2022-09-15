#include "core/autocomplete.h"
#include "core/io.h"
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
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wait.h>
#include <assert.h>
#include <termios.h>
#include <ctype.h>

int commandCount;
Command commandArr[15];
char currentPath[MAX_PATH_SIZE];
char home[MAX_PATH_SIZE];
int lastCommandStatus;
long int lastCommandTime;
int bgProcessesRunning;
int shouldExitShell;
struct termios termiosAttr, defTermiosAttr;

int cmdLength = 0;
char cmd[MAX_CMD_LENGTH];

void zombie_handler(int sig, siginfo_t* info, void* ucontext)
{
    int status = 0;
    pid_t p = -1;
    while((p = waitpid(-1, &status, WUNTRACED | WNOHANG)) > 0)
    {
        print("halo\n");
        const char* processName = getProcessNameByPID(p);
        if(processName == NULL) continue;

        lastCommandStatus = 0;
        const char* sta = "exited abnormally";
        if(WIFSIGNALED(status))
        {
            sta = "was killed";
            lastCommandStatus = WTERMSIG(status);
        } else if(WIFEXITED(status))
        {
            sta = "exited normally";
            lastCommandStatus = WEXITSTATUS(status);
        } else if(WIFSTOPPED(status))
        {
            sta = "suspended normally";
            lastCommandStatus = WSTOPSIG(status);
        }
        if(processName != NULL)
        {
            print("\n%s with pid = %d %s\n", processName, p, sta);
            setProcessStatus(p, WIFSTOPPED(status) ? 1 : 0);
            if(!WIFSTOPPED(status))
            {
                removeProcess(p);
                bgProcessesRunning--;
            }
        }
    }
    if(tcgetpgrp(STDIN_FILENO) == getpid())
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &termiosAttr);
        if(render_prompt())
        {
            for(int i = 0; i < cmdLength; i++) print("%c", cmd[i]);
            fflush(stdout);
        }
    }
}

int main ()
{
    int shell;
    while (tcgetpgrp (STDIN_FILENO) != (shell = getpgrp ()))
        kill (- shell, SIGTTIN);
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
    signal(SIGINT, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);

    if(initHistory() == -1) return -1;

    getWorkingDir();
    strcpy(home, currentPath);

    useBuiltins();

    while(!shouldExitShell)
    {
        render_prompt();
        cmdLength = 0;
        while(1)
        {
            if(read(STDIN_FILENO, &cmd[cmdLength], 1) == -1)
            {
                LogPError("read");
                // break;
                return -1;
            }
            char ch = cmd[cmdLength];
            switch (ch) {
                case '\004': // eof
                {
                    killAllProcesses();
                    return 0;
                }
                case '\x1b': // arrow keys
                {
                    char buf[3];
                    buf[2] = 0;
                    read(STDIN_FILENO, buf, 2);
                    continue;
                }
                case '\t':
                {
                    // if(cmdLength == 0) continue;
                    cmd[cmdLength] = '\0';
                    cmdLength = autocomplete(cmdLength, cmd);
                    continue;
                }
                case '\177':
                {
                    if(cmdLength > 0)
                    {
                        cmdLength--;
                        print("\b \b");
                    }
                    continue;
                }
                default:
                {
                    if(cmd[cmdLength] == '\n' || !iscntrl(cmd[cmdLength])) print("%c", cmd[cmdLength]);
                }
            }
            if(cmd[cmdLength] == '\n')
            {
                cmd[cmdLength] = '\0';
                cmdLength = 0;
                loadHistory();
                recordInHistory(cmd);
                parse(cmd);
                saveHistory();
                break;
            }
            if(cmd[cmdLength] == '\n' || !iscntrl(cmd[cmdLength])) cmdLength++;
        }
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &defTermiosAttr);
    killAllProcesses();
    return 0;
}
