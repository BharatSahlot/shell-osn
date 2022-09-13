#include "autocomplete.h"
#include "../globals.h"
#include "../core/prompt.h"

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

int autocomplete(int n, char *buf)
{
    DIR* dir = opendir(currentPath);
    if(dir == NULL)
    {
        LogPError("dsa");
        return n;
    }

    char* origBuf = buf;
    int ln = 0;
    for(int i = n - 1; i >= 0; i--)
    {
        if(buf[i] == ' ')
        {
            ln = i + 1;
            n -= i + 1;
            buf = &buf[i + 1];
            break;
        }
    }

    int lines = 0;
    const char* lastItem = NULL;
    struct dirent* item = readdir(dir);
    while(item != NULL)
    {
        if(strncmp(buf, item->d_name, n) != 0)
        {
            item = readdir(dir);
            continue;
        }
        lines++;
        lastItem = item->d_name;
        item = readdir(dir);
    }

    if(lines == 0) return n;

    if(lines == 1)
    {
        strcpy(buf, lastItem);
        closedir(dir);

        print("\x1b[%dD", n);
        n = strlen(lastItem);
        
        int r = isDir(lastItem);
        if(r == 1) // dont do just if(r), isDir can return -1
        {
            buf[n++] = '/';
        } else if(r == 0)
        {
            buf[n++] = ' ';
        }

        for(int i = 0; i < n; i++)
        {
            print("%c", buf[i]);
        }

        fflush(stdout);
        return ln + n;
    }

    int found = 0;
    static char tmp[MAX_PATH_SIZE];
    tmp[0] = '\0';

    seekdir(dir, 0);
    item = readdir(dir);
    while(item != NULL)
    {
        if(strncmp(buf, item->d_name, n) != 0)
        {
            item = readdir(dir);
            continue;
        }
        if(!found)
        {
            strcpy(tmp, item->d_name);
            found = 1;
            item = readdir(dir);
            continue;
        }

        int n1 = strlen(item->d_name), n2 = strlen(tmp);
        int mn = n1 < n2 ? n1 : n2;
        for(int i = n; i < mn; i++)
        {
            if(item->d_name[i] != tmp[i])
            {
                tmp[i] = '\0';
                break;
            }
        }
        item = readdir(dir);
    }
    strcpy(buf, tmp);
    print("\x1b[%dD", n);
    n = strlen(tmp);

    for(int i = 0; i < n; i++)
    {
        print("%c", buf[i]);
    }
    fflush(stdout);

    seekdir(dir, 0);
    item = readdir(dir);
    while(item != NULL)
    {
        if(strncmp(buf, item->d_name, n) != 0)
        {
            item = readdir(dir);
            continue;
        }
        print("\n%s", item->d_name);
        item = readdir(dir);
    }
    closedir(dir);
    print("\n");
    render_prompt();
    for(int i = 0; i < ln + n; i++) print("%c", origBuf[i]);
    fflush(stdout);
    return ln + n;
}
