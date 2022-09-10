#include "prompt.h"
#include "io.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>

const char* username = NULL;
char sysname[25] = "\0";

int linesPrintedTillLastPrompt = -1;
int lastPromptLength = 0;

int getLastPromptLength()
{
    return lastPromptLength;
}

int render_prompt()
{
    // dont render prompt if nothing has been written to the standard out
    if(linesPrintedTillLastPrompt == getPrintedLinesCount()) return 0;

    if(username == NULL)
    {
        username = getUsernameFromId(geteuid());
        if(username == NULL)
        {
            LogPError("prompt");
            return 0;
        }
    }

    if(sysname[0] == '\0')
    {
        if(gethostname(sysname, 25) == -1)
        {
            LogPError("prompt");
            return 0;
        }
    }

    const char* prefixColor = lastCommandStatus == 0 ? GRAY : RED;
    const char* dirColor = YELLOW;
    if(lastCommandTime > 0)
    {
        int c = 0, t = lastCommandTime;
        while(t > 0)
        {
            c++;
            t /= 10;
        }
        const char* pth = addTildaToPath(currentPath);
        lastPromptLength = 11 + strlen(username) + strlen(sysname) + strlen(pth) + c;
        print("%s<%s@%s:%s%s%s took %lds%s>%s ", prefixColor, username, sysname, dirColor, pth, CYAN, lastCommandTime, prefixColor, RESET);
        lastCommandTime = 0;
    } else
    {
        const char* pth = addTildaToPath(currentPath);
        lastPromptLength = 5 + strlen(username) + strlen(sysname) + strlen(pth);
        print("%s<%s@%s:%s%s%s>%s ", prefixColor, username, sysname, dirColor, pth, prefixColor, RESET);
    }
    linesPrintedTillLastPrompt = getPrintedLinesCount();
    return 1;
}
