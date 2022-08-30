#include "cmds.h"
#include "../logger.h"
#include "../utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>

int cmp(const void* _a, const void* _b)
{
    const char* a = *(const char**)_a;
    const char* b = *(const char**)_b;
    return strcmp(a, b);
}

int getLastItemFromPath(const char* path)
{
    int n = strlen(path);
    for(int i = n - 1; i >= 0; i--)
    {
        if(path[i] == '/') return i + 1;
    }
    return 0;
}

char** getItemsInDir(const char* path, int* outCount, int includeHidden, int sorted)
{
    char** res = NULL;
    DIR* dir = opendir(path);

    *outCount = -1;
    errno = 0;
    int count = 0;
    struct dirent* item = readdir(dir);
    while(item != NULL)
    {
        if(includeHidden || item->d_name[0] != '.') count++;
        item = readdir(dir);
    }
    if(errno) return NULL;

    seekdir(dir, 0);
    item = readdir(dir);
    res = (char**) malloc(count * sizeof(char**));
    for(int i = 0; i < count; i++) res[i] = NULL;

    for(int i = 0; i < count;)
    {
        if(includeHidden || item->d_name[0] != '.')
        {
            res[i] = (char*) malloc(strlen(item->d_name) * sizeof(char));
            strcpy(res[i], item->d_name);
            i++;
        }
        item = readdir(dir);
    }
    if(errno)
    {
        for(int i = 0; i < count; i++) if(res[i]) free(res[i]);
        free(res);
        return NULL;
    }
    if(sorted)
    {
        // if(includeHidden) qsort(res + 2, count - 2, sizeof(char*), cmp);
        qsort(res, count, sizeof(char*), cmp);
    }
    *outCount = count;
    return res;
}

int genPermString(mode_t st_mode, char* out)
{
    for(int i = 0; i < 10; i++) out[i] = '-';

    if(S_ISDIR(st_mode))  out[0] = 'd';
    if(st_mode & S_IRUSR) out[1] = 'r';
    if(st_mode & S_IWUSR) out[2] = 'w';
    if(st_mode & S_IXUSR) out[3] = 'x';
    if(st_mode & S_IRGRP) out[4] = 'r';
    if(st_mode & S_IWGRP) out[5] = 'w';
    if(st_mode & S_IXGRP) out[6] = 'x';
    if(st_mode & S_IROTH) out[7] = 'r';
    if(st_mode & S_IWOTH) out[8] = 'w';
    if(st_mode & S_IXOTH) out[9] = 'x';

    return 10;
}

void genlsLine(const char* path, struct stat* st, char* out)
{
    int n = genPermString(st->st_mode, out);
    n += sprintf(out + n, " %lu", st->st_nlink);
    out[n++] = ' ';

    const char* user = getUserNameFromId(st->st_uid);
    if(user == NULL)
    {
        LogPError("ls");
        return;
    }

    strcpy(out + n, user);
    n += strlen(user);
    out[n++] = ' ';
    
    const char* group = getGroupName(st->st_gid);
    if(group == NULL)
    {
        LogPError("ls");
    }
    strcpy(out + n, group);
    n += strlen(group);
    n += sprintf(out + n, " %4lu ", st->st_size);
    n += strftime(out + n, 13, "%h %d %R", localtime(&st->st_mtime));
    out[n++] = ' ';

    const char* pi = path + getLastItemFromPath(path);
    if(S_ISDIR(st->st_mode))
    {
        sprintf(out + n, "\033[1;34m%s\033[0m", pi);
        n += 11;
    } else
    {
        sprintf(out + n, "%s", pi);
    }
    // strcpy(out + n, path);
    n += strlen(pi);
    out[n] = '\0';
}

// prints ls for a single dir/file
void lss(char* path, int displayHiddenFiles, int displayExtraInfo)
{
    if(isFile(path))
    {
        struct stat st;
        int err = stat(path, &st);
        if(err == -1)
        {
            LogPError("ls");
            return;
        }
        if(displayExtraInfo)
        {
            char line[100];
            genlsLine(path, &st, line);
            printf("%s\n", line);
        } else
        {
            const char* pi = path + getLastItemFromPath(path);
            printf("%s\n", pi);
        }
    } else if(isDir(path) == 1)
    {
        int count = 0;
        char** items = getItemsInDir(path, &count, displayHiddenFiles, 1);
        if(items == NULL)
        {
            LogPError("ls");
            return;
        }

        if(!displayExtraInfo)
        {
            for(int i = 0; i < count; i++)
            {
                if(isDir(items[i]) == 1)
                {
                    printf("\033[1;34m%s\033[0m\n", items[i]);
                } else if(isExecutable(items[i]) == 1)
                {
                    printf("\033[1;32m%s\033[0m\n", items[i]);
                } else printf("%s\n", items[i]);
            }
            free(items);
            return;
        }

        char** buf = (char**) malloc(sizeof(char**) * count);
        for(int i = 0; i < count; i++) buf[i] = NULL;

        char tp[250];
        struct stat st;
        for(int i = 0; i < count; i++)
        {
            buf[i] = (char*) malloc(sizeof(char*) * 150);

            strcpy(tp, path);
            joinPaths(tp, items[i]);

            int err = stat(tp, &st);
            if(err == -1)
            {
                for(int j = 0; j <= i; j++) free(buf[j]);
                free(buf);
                free(items);
                LogPError("ls");
                return;
            }
            genlsLine(tp, &st, buf[i]);
        }

        for(int i = 0; i < count; i++)
        {
            printf("%s\n", buf[i]);
        }
        for(int j = 0; j < count; j++) free(buf[j]);
        free(buf);
        free(items);
    } else
    {
        Log(LOGL_ERROR, "ls: No such file or directory %s\n", path);
    }
}

void ls(int argc, char **argv)
{
    int displayHiddenFiles = 0;
    int displayExtraInfo = 0;
    int count = 0;

    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            count++;
            continue;
        }

        if(strcmp(argv[i], "-a") == 0) displayHiddenFiles = 1;
        else if(strcmp(argv[i], "-l") == 0) displayExtraInfo = 1;
        else if(strcmp(argv[i], "-al") == 0 || strcmp(argv[i], "-la") == 0) displayExtraInfo = displayHiddenFiles = 1;
        else
        {
            Log(LOGL_ERROR, "ls: Unsupported arguement %s\n", argv[i]);
            return;
        }
    }
    
    int printed = 0;
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-') continue;
        if(count > 1)
        {
            if(printed) printf("\n");
            printed = 1;

            if(isDir(argv[1]) == 1) printf("%s:\n", argv[i]);
        }
        lss(argv[i], displayHiddenFiles, displayExtraInfo);
    }
}
