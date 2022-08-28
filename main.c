#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "logger.h"

int main (int argc, char *argv[])
{
    Log(LOGL_INFO, "Info: %d %d", 10, 10);
    Log(LOGL_WARN, "Warn: %d %d", 10, 10);
    Log(LOGL_ERROR, "Error: %d %d", 10, 10);
    return 0;
}
