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

    printf(GRAY "<%s@%s:" YELLOW "%s" GRAY "> " RESET, username, sysname, addTildaToPath(currentPath));
}
