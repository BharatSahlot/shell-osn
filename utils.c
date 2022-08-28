#include "utils.h"

#include "logger.h"

#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>

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
