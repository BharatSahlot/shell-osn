#include "utils.h"

#include "logger.h"

#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <sys/stat.h>

struct utsname uts;

const char* getUserName()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) return pw->pw_name;
    return "Error";
}

const char* getSystemName()
{
    if(uname(&uts) != 0)
    {
        Log(LOGL_ERROR, "Cannot get system name");
        return "system";
    }
    return uts.sysname;
}

int checkIfDirectoryExists(const char* dir)
{
    struct stat s;
    int err = stat(dir, &s);
    if(err == -1) return -1;
    return S_ISDIR(s.st_mode);
}
