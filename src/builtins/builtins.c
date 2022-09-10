#include "builtins.h"

void useBuiltins()
{
    commandArr[commandCount].cmd = "ls";
    commandArr[commandCount++].func = ls;

    commandArr[commandCount].cmd = "cd";
    commandArr[commandCount++].func = cd;

    commandArr[commandCount].cmd = "pwd";
    commandArr[commandCount++].func = pwd;

    commandArr[commandCount].cmd = "echo";
    commandArr[commandCount++].func = echo;

    commandArr[commandCount].cmd = "pinfo";
    commandArr[commandCount++].func = pinfo;

    commandArr[commandCount].cmd = "history";
    commandArr[commandCount++].func = history;

    commandArr[commandCount].cmd = "discover";
    commandArr[commandCount++].func = discover;

    commandArr[commandCount].cmd = "jobs";
    commandArr[commandCount++].func = jobs;

    commandArr[commandCount].cmd = "sig";
    commandArr[commandCount++].func = sig;
}
