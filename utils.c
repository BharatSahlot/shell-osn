#include "utils.h"

#include "logger.h"

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <grp.h>

struct utsname uts;
char sysName[50];

char currentDir[250];

const char* getCurrentDir()
{
    const char* res = getcwd(currentDir, 250);
    return res;
}

const char* getUserName()
{
    return getUserNameFromId(geteuid());
}

const char* getUserNameFromId(uid_t id)
{
    errno = 0;
    struct passwd* pw = getpwuid(id);
    if(pw != NULL) return pw->pw_name;
    return NULL;
}

const char* getGroupName(gid_t gid)
{
    errno = 0; // need to do this if want to check for errors. see man 3 getgrnam
    struct group* gr = getgrgid(gid);
    if(gr != NULL) return gr->gr_name;
    return NULL;
}

const char* getSystemName()
{
    int err = gethostname(sysName, 50);
    if(err == -1) return NULL;
    return sysName;
}

int isDir(const char* dir)
{
    struct stat s;
    int err = stat(dir, &s);
    if(err == -1) return -1;
    return S_ISDIR(s.st_mode);
}

int isFile(const char* dir)
{
    struct stat s;
    int err = stat(dir, &s);
    if(err == -1) return -1;
    return S_ISREG(s.st_mode);
}

int isExecutable(const char* dir)
{
    struct stat s;
    int err = stat(dir, &s);
    if(err == -1) return -1;
    return S_ISREG(s.st_mode) && (s.st_mode & S_IXUSR);
}

void moveUpDirectory(char *path)
{
    int n = strlen(path);
    for(int i = n - 1; i >= 0; --i)
    {
        if(path[i] == '/')
        {
            path[i] = '\0';
            break;
        }
    }
}

void joinPaths(char* p1, char* p2)
{
    int n = strlen(p1);
    p1[n] = '/';
    if(*p2 == '/') ++p2;

    int n1 = strlen(p2);

    // paths shouldn't end with /
    if(p2[n1 - 1] == '/') p2[n1 - 1] = '\0';

    strcpy(p1 + n + 1, p2);
}
