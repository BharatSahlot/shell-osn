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

    int pi = 0, pn = strlen(path);
    for(pi = pn; pi >= 0; pi--)
    {
        if(path[pi] == '/') break;
    }
    if(S_ISDIR(st->st_mode))
    {
        sprintf(out + n, "\033[1;34m%s\033[0m", path + pi + 1);
        n += 11;
    } else
    {
        sprintf(out + n, "%s", path + pi + 1);
    }
    // strcpy(out + n, path);
    n += strlen(path + pi + 1);
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
            printf("%s\n", path);
        }
    } else if(isDir(path))
    {
        DIR* dir = opendir(path);
        if(dir == NULL)
        {
            LogPError("ls");
            return;
        }
        errno = 0;

        int pathLen = strlen(path);
        char tp[250];
        strcpy(tp, path);

        int lines = 0;
        struct dirent* item = readdir(dir);
        while(item != NULL)
        {
            if(displayHiddenFiles || item->d_name[0] != '.') ++lines;
            item = readdir(dir);
        }
        if(errno)
        {
            LogPError("ls");
            closedir(dir);
            return;
        }
        seekdir(dir, 0);
        item = readdir(dir);

        struct stat st;

        char** buf = (char**) malloc(lines * sizeof(char*));
        for(int i = 0; i < lines; i++) buf[i] = NULL;

        int j = 0;
        for(int i = 0; i < lines && item != NULL; i++)
        {
            if(displayHiddenFiles || item->d_name[0] != '.')
            {
                strcpy(tp, path);
                joinPaths(tp, item->d_name);

                buf[j] = (char*) malloc(sizeof(char) * 250);
                
                stat(tp, &st);
                genlsLine(tp, &st, buf[j++]);
            }

            item = readdir(dir);
        }
        if(errno) LogPError("ls");
        for(int i = 0; i < lines; i++)
        {
            if(buf[i] == NULL) continue;
            printf("%s\n", buf[i]);
            free(buf[i]);
        }
        free(buf);
        closedir(dir);
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
    
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-') continue;
        if(count > 1) printf("%s:\n", argv[i]);
        lss(argv[i], displayHiddenFiles, displayExtraInfo);
    }
}
