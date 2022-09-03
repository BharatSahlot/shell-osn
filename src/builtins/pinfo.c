#include "builtins.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int pinfo(int argc, const char *argv[])
{
    pid_t pid = getpid();
    if(argc > 1)
    {
        pid = atoi(argv[1]);
    }

    char path[20];
    sprintf(path, "/proc/%d/stat", pid);

    int fd = open(path, O_RDONLY);
    if(fd == -1)
    {
        LogPError("pinfo");
        return -1;
    }
    char buffer[100];
    int n = read(fd, buffer, 100);
    close(fd);

    int id;
    char executablePath[100], state;
    sscanf(buffer, "%d%s %c", &id, executablePath, &state);

    sprintf(path, "/proc/%d/exe", pid);
    n = readlink(path, buffer, 99);
    if(n == -1)
    {
        LogPError("pinfo");
        return -1;
    }
    buffer[n] = '\0';

    const char* tp = addTildaToPath(buffer);
    strcpy(executablePath, tp);

    sprintf(path, "/proc/%d/statm", pid);
    fd = open(path, O_RDONLY);
    if(fd == -1)
    {
        LogPError("pinfo");
        return -1;
    }
    n = read(fd, buffer, 100);
    close(fd);

    int vmSize = 0;
    sscanf(buffer, "%d", &vmSize);

    printf("pid : %d\n", pid);
    printf("process status : %c\n", state);
    printf("memory : %d\n", vmSize);
    printf("executable path : %s\n", executablePath);
    return 0;
}
