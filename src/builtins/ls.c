#include "builtins.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_ITEMS_IN_DIR 1000
#define MAX_ITEM_LENGTH 50

int items = 0;
char itemsBuffer[MAX_ITEMS_IN_DIR][MAX_ITEM_LENGTH];

int cmp(const void* _a, const void* _b)
{
    const char* a = (const char *)_a;
    const char* b = (const char *)_b;

    while(*a == '.') a++;
    while(*b == '.') b++;

    while(*a != '\0' && *b != '\0')
    {
        char ac = *a, bc = *b;
        if(ac == bc)
        {
            a++;
            b++;
            continue;
        }
        if(tolower(ac) == tolower(bc)) return ac > bc ? -1 : 1;
        return tolower(ac) < tolower(bc) ? -1 : 1;
    }
    if(*a == '\0' && *b == '\0') return 0;
    return *a != '\0' ? 1 : -1;
}

// expects that the path is formatted properly, returns -1 on error
int readItemsInDir(const char* path, int includeHidden)
{
    items = 0;
    DIR* dir = opendir(path);
    
    errno = 0;
    struct dirent* item = readdir(dir);
    while(item != NULL)
    {
        if(includeHidden || item->d_name[0] != '.')
        {
            strcpy(itemsBuffer[items++], item->d_name);
        }
        item = readdir(dir);
    }
    if(errno != 0)
    {
        LogPError("ls");
        closedir(dir);
        return -1;
    }
    char temp[MAX_ITEM_LENGTH];
    qsort(itemsBuffer, items, sizeof(itemsBuffer[0]), cmp);

    for(int i = 0; i < items; i++)
    {
        int pn = strlen(path);
        strcpy(temp, itemsBuffer[i]);
        strcpy(itemsBuffer[i], path);
        itemsBuffer[i][pn] = '/';
        strcpy(itemsBuffer[i] + pn + 1, temp);
    }
    closedir(dir);
    return 0;
}

int genPermString(mode_t st_mode, char* out)
{
    for(int i = 0; i < 10; i++) out[i] = '-';

    switch (st_mode & S_IFMT) {
        case S_IFREG: out[0] = '-'; break;
        case S_IFBLK: out[0] = 'b'; break;
        case S_IFCHR: out[0] = 'c'; break;
        case S_IFDIR: out[0] = 'd'; break;
        case S_IFLNK: out[0] = 'l'; break;
        case S_IFIFO: out[0] = 'p'; break;
        case S_IFSOCK: out[0] = 's'; break;
    }

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

int isExecutable(const struct stat* st)
{
    return (st->st_mode & (S_IXUSR | S_IXGRP | S_IXOTH));
}

void printfItemColor(const char* item, const struct stat* st)
{
    if(S_ISDIR(st->st_mode)) printf(BOLD BLUE "%s" RESET, item);
    else if(S_ISLNK(st->st_mode)) printf(BOLD CYAN "%s" RESET, item);
    else if(isExecutable(st)) printf(BOLD GREEN "%s" RESET, item);
    else printf("%s", item);
}

// on success returns blocks occupied by the item
int getItemLine(const char* path, char* line)
{
    struct stat st;
    int err = lstat(path, &st);
    if(err == -1)
    {
        LogPError("ls");
        return -1;
    }

    int n = genPermString(st.st_mode, line);
    const char* user = getUsernameFromId(st.st_uid);
    if(user == NULL)
    {
        LogPError("ls");
        return -1;
    }

    const char* group = getGroupnameFromId(st.st_gid);
    if(group == NULL)
    {
        LogPError("ls");
        return -1;
    }
    n += sprintf(line + n, " %4lu %s %s %8lu ", st.st_nlink, user, group, st.st_size);
    n += strftime(line + n, 14, "%h %d %R ", localtime(&st.st_mtime));

    // to only print the last word of the path
    const char* ptr = path + strlen(path) - 1;
    while(ptr >= path)
    {
        if(*ptr == '/') break;
        --ptr;
    }
    if(*ptr == '/') ++ptr;

    if(S_ISLNK(st.st_mode))
    {
        char buf[50];
        err = readlink(path, buf, 50);
        if(err == -1)
        {
            LogPError("ls");
            return -1;
        }
        sprintf(line + n, BOLD CYAN "%s" RESET " -> " BOLD BLUE "%s" RESET, ptr, buf);
        return st.st_blocks;
    }
    if(S_ISDIR(st.st_mode)) n += sprintf(line + n, BOLD BLUE "%s" RESET, ptr);
    else if(isExecutable(&st)) n += sprintf(line + n, BOLD GREEN "%s" RESET, ptr);
    else n += sprintf(line + n, "%s", ptr);
    return st.st_blocks;
}

int lsItem(const char* path, int displayHiddenFiles, int displayExtraInfo)
{
    struct stat st;
    int err = lstat(path, &st);
    if(err == -1)
    {
        LogPError("ls");
        return -1;
    }

    if(S_ISDIR(st.st_mode))
    {
        err = readItemsInDir(path, displayHiddenFiles);
        if(err == -1) return -1;

        if(!displayExtraInfo)
        {
            for(int i = 0; i < items; i++)
            {
                const char* ptr = itemsBuffer[i] + strlen(itemsBuffer[i]) - 1;
                while(ptr > itemsBuffer[i])
                {
                    if(*ptr == '/') break;
                    --ptr;
                }
                if(*ptr == '/') ++ptr;

                err = lstat(itemsBuffer[i], &st);
                if(err == -1)
                {
                    LogPError("ls");
                    continue;
                }

                printfItemColor(ptr, &st);
                printf("\n");
            }
            return 0;
        }

        int totalBlocks = 0;

        int line = 0;
        char lines[MAX_ITEMS_IN_DIR][250];
        for(int i = 0; i < items; i++)
        {
            err = getItemLine(itemsBuffer[i], lines[line]);
            if(err == -1) continue;

            totalBlocks += err;
            line++;
        }
        printf("total %d\n", totalBlocks / 2);
        for(int i = 0; i < items; i++)
        {
            printf("%s\n", lines[i]);
        }
    } else
    {
        if(displayExtraInfo)
        {
            char line[250];
            getItemLine(path, line);
            printf("%s\n", line);
        } else
        {
            const char* ptr = path + strlen(path) - 1;
            while(ptr >= path)
            {
                if(*ptr == '/') break;
                --ptr;
            }
            if(*ptr == '/') ++ptr;

            printfItemColor(ptr, &st);
            printf("\n");
        }
    }
    return 0;
}

int ls(int argc, const char **argv)
{
    items = 0;
    int count = 0; // count of no. of paths in the args
    int listExtraInfo = 0, listHiddenItems = 0;
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] != '-')
        {
            count++;
            continue;
        }
        const char* ptr = argv[i] + 1;
        while(*ptr != '\0')
        {
            if(*ptr == 'a') listHiddenItems = 1;
            else if(*ptr == 'l') listExtraInfo = 1;
            else
            {
                Log(LOGL_ERROR, "ls: invalid flag %c specified\n", *ptr);
                return -1;
            }
            ptr++;
        }
    }
    if(count == 0)
    {
        int err = lsItem(currentPath, listHiddenItems, listExtraInfo);
        if(err == -1) return -1;
        return 0;
    }

    int c = 0; // temp index
    char temp[MAX_ARGS][MAX_ITEM_LENGTH];
    struct stat st;
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-') continue;

        if(lstat(argv[i], &st) == -1)
        {
            LogPError("ls");
            continue;
        }
        if(S_ISDIR(st.st_mode)) continue;
        strcpy(temp[c++], argv[i]);
    }

    int filesPrinted = 0;
    qsort(temp, c, sizeof(temp[0]), cmp);
    for(int i = 0; i < c; i++)
    {
        filesPrinted = 1;
        lsItem(temp[i], listHiddenItems, listExtraInfo);
    }

    c = 0;
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '-') continue;

        if(lstat(argv[i], &st) == -1)
        {
            LogPError("ls");
            continue;
        }
        if(!S_ISDIR(st.st_mode)) continue;
        strcpy(temp[c++], argv[i]);
    }
    qsort(temp, c, sizeof(temp[0]), cmp);
    for(int i = 0; i < c; i++)
    {
        if(i || filesPrinted) printf("\n");
        if(count > 1) printf("%s:\n", temp[i]);
        lsItem(temp[i], listHiddenItems, listExtraInfo);
    }
    return 0;
}
