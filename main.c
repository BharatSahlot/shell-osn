#include <stdio.h>

#include "logger.h"
#include "utils.h"

char prompt[50];
char dir[50] = "~";

int main (int argc, char *argv[])
{
    sprintf(prompt, "%s@%s", getUserName(), getSystemName());
    printf("<%s:%s>", prompt, dir);
    return 0;
}
