#include "prompt.h"

#include <stdio.h>
#include <unistd.h>

const char* username = NULL;
char sysname[25] = "\0";

void render_prompt()
{
    if(username == NULL)
    {
        username = getUsernameFromId(geteuid());
        if(username == NULL)
        {
            LogPError("prompt");
            return;
        }
    }

    if(sysname[0] == '\0')
    {
        if(gethostname(sysname, 25) == -1)
        {
            LogPError("prompt");
            return;
        }
    }

    const char* prefixColor = lastCommandStatus == 0 ? GRAY : RED;
    const char* dirColor = YELLOW;
    if(lastCommandTime > 0)
    {
        printf("%s<%s@%s:%s%s%s took %ds%s>%s", prefixColor, username, sysname, dirColor, addTildaToPath(currentPath), CYAN, lastCommandTime, prefixColor, RESET);
        lastCommandTime = 0;
    } else
    {
        printf("%s<%s@%s:%s%s%s>%s", prefixColor, username, sysname, dirColor, addTildaToPath(currentPath), prefixColor, RESET);
    }
}
