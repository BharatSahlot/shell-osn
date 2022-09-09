#include "autocomplete.h"
#include "../globals.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

void autocomplete(int n, char *buf)
{
    DIR* dir = opendir(currentPath);
    if(dir == NULL)
    {
        LogPError("dsa");
        return;
    }

    int lines = 0;
    struct dirent* item = readdir(dir);
    while(item != NULL)
    {
        if(strncmp(buf, item->d_name, n) != 0)
        {
            item = readdir(dir);
            continue;
        }
        lines++;
        printf("\n%s", item->d_name);
        item = readdir(dir);
    }
    closedir(dir);
}
