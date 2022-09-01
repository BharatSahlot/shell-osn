#include "cmds.h"
#include "../logger.h"
#include "../utils.h"

#include <stdio.h>
#include <unistd.h>

void pwd(int argc, char* argv[])
{
    if(argc > 1)
    {
        Log(LOGL_ERROR, "pwd: too many arguments\n");
        return;
    }
    const char* dir = getCurrentDir();
    if(dir == NULL)
    {
        LogPError("pwd");
        return;
    }
    printf("%s\n", dir);
}
