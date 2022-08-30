#include "cmds.h"
#include "../logger.h"
#include "../utils.h"

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>

int genPermString(mode_t st_mode, char* out)
{
    for(int i = 0; i < 11; i++) out[i] = '-';

    if(S_ISDIR(st_mode)) out[1] = 'd';
    if(st_mode & S_IRUSR) out[2] = 'r';
    if(st_mode & S_IWUSR) out[3] = 'w';
    if(st_mode & S_IXUSR) out[4] = 'x';
    if(st_mode & S_IRGRP) out[5] = 'r';
    if(st_mode & S_IWGRP) out[6] = 'w';
    if(st_mode & S_IXGRP) out[7] = 'x';
    if(st_mode & S_IROTH) out[8] = 'r';
    if(st_mode & S_IWOTH) out[9] = 'w';
    if(st_mode & S_IXOTH) out[10] = 'x';

    return 11;
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
    n += sprintf(out + n, " %lu ", st->st_size);
    n += strftime(out + n, 13, "%h %d %R", localtime(&st->st_mtime));
    out[n++] = ' ';
    strcpy(out + n, path);
    n += strlen(path);
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
