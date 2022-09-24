#include "builtins.h"

void useBuiltins()
{
    commandArr[commandCount].cmd = "ls";
    commandArr[commandCount].runInPipe = 1;
    commandArr[commandCount++].func = ls;

    commandArr[commandCount].cmd = "cd";
    commandArr[commandCount].runInPipe = 0;
    commandArr[commandCount++].func = cd;

    commandArr[commandCount].cmd = "pwd";
    commandArr[commandCount].runInPipe = 1;
    commandArr[commandCount++].func = pwd;

    commandArr[commandCount].cmd = "echo";
    commandArr[commandCount].runInPipe = 1;
    commandArr[commandCount++].func = echo;

    commandArr[commandCount].cmd = "pinfo";
    commandArr[commandCount].runInPipe = 1;
    commandArr[commandCount++].func = pinfo;

    commandArr[commandCount].cmd = "history";
    commandArr[commandCount].runInPipe = 1;
    commandArr[commandCount++].func = history;

    commandArr[commandCount].cmd = "discover";
    commandArr[commandCount].runInPipe = 1;
    commandArr[commandCount++].func = discover;

    commandArr[commandCount].cmd = "jobs";
    commandArr[commandCount].runInPipe = 1;
    commandArr[commandCount++].func = jobs;

    commandArr[commandCount].cmd = "sig";
    commandArr[commandCount].runInPipe = 1;
    commandArr[commandCount++].func = sig;

    commandArr[commandCount].cmd = "fg";
    commandArr[commandCount].runInPipe = 0;
    commandArr[commandCount++].func = fg;

    commandArr[commandCount].cmd = "bg";
    commandArr[commandCount].runInPipe = 1;
    commandArr[commandCount++].func = bg;
}
