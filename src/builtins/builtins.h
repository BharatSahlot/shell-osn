#ifndef BUILTINS_H
#define BUILTINS_H

#include "../globals.h"

void useBuiltins();

extern int ls(int argc, const char* argv[]);
extern int cd(int argc, const char* argv[]);
extern int pwd(int argc, const char* argv[]);
extern int echo(int argc, const char* argv[]);

#endif