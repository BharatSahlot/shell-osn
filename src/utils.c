#include "utils.h"
#include "globals.h"

#include <pwd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <grp.h>

#include <stdio.h>

char tempPathBuf[MAX_PATH_SIZE], tempPathBuf2[MAX_PATH_SIZE];

int isDir(const char* path)
{
    static struct stat st;
    if(stat(path, &st) == -1)
    {
        return -1;
    }
    return S_ISDIR(st.st_mode);
}

// returns username if successfull else NULL
const char* getUsernameFromId(uid_t uid)
{
    struct passwd* pw = getpwuid(uid);
    if(pw == NULL) return NULL;
    return pw->pw_name;
}

// returns groupname if successfull else NULL
const char* getGroupnameFromId(gid_t gid)
{
    errno = 0; // need to do this if want to check for errors. see man 3 getgrnam
    struct group* gr = getgrgid(gid);
    if(gr != NULL) return gr->gr_name;
    return NULL;
}

// returns the current working dir or NULL
const char* getWorkingDir()
{
    const char* ptr = getcwd(currentPath, MAX_PATH_SIZE);
    return ptr;
}

// adds ~ to the path beginning if HOME is a prefix of it
const char* addTildaToPath(const char* path)
{
    strcpy(tempPathBuf, path);
    if(strncmp(tempPathBuf, home, strlen(home)) == 0)
    {
        int hn = strlen(home);
        int pn = strlen(tempPathBuf);

        memmove(tempPathBuf + 1, tempPathBuf + hn,  pn - hn);
        tempPathBuf[0] = '~';
        tempPathBuf[pn - hn + 1] = '\0';
    }
    return tempPathBuf;
}

// makes the path absolute
const char* makePathAbsolute(const char* path)
{
    if(path[0] == '~')
    {
        sprintf(tempPathBuf, "%s%s", home, path + 1);
    } else strcpy(tempPathBuf, path);

    if(tempPathBuf[0] == '/') return tempPathBuf;
    sprintf(tempPathBuf2, "%s/%s", getWorkingDir(), tempPathBuf);
    return tempPathBuf2;
}

// joins two paths which dont contain ., .. and ~
void joinPaths(char *p1, const char *p2)
{
    int pn = strlen(p1);
    if(p1[pn - 1] == '/') pn--;
    if(p2[0] != '/') p1[pn++] = '/';
    strcpy(p1 + pn, p2);
}
