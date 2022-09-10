#ifndef GLOBALS_H
#define GLOBALS_H

#include "core/io.h"
#include "utils.h"
#include "logger.h"

#define MAX_PATH_SIZE 4096
#define MAX_ARGS 10
#define MAX_CMD_LENGTH MAX_PATH_SIZE * MAX_ARGS
#define HCOMMANDS_TO_STORE 20
#define HCOMMANDS_TO_DISPLAY 10

#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define PINK "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define GRAY "\033[90m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

typedef struct
{
    const char* cmd;
    int (*func) (int argc, const char* argv[]);
} Command;

extern int commandCount;
extern Command commandArr[10];

extern int historyCount;
extern char historyArr[HCOMMANDS_TO_STORE][MAX_CMD_LENGTH];

// Current absolute path
extern char currentPath[MAX_PATH_SIZE];

// HOME path, the path where the shell was executed in
extern char home[MAX_PATH_SIZE];

extern int shouldExitShell;

// Status code of last command/process executed
extern int lastCommandStatus;

// Time taken by last command in seconds
extern long int lastCommandTime;

extern struct termios termiosAttr;
extern struct termios defTermiosAttr;

#endif
