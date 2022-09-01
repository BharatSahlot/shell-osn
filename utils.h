#ifndef UTILS_H
#define UTILS_H

#include <unistd.h>

const char* getCurrentDir();
const char* getUserName();
const char* getUserNameFromId(uid_t id);

const char* getGroupName(gid_t gid);

const char* getSystemName();

int isDir(const char* dir);
int isFile(const char* dir);
int isExecutable(const char* dir);
void moveUpDirectory(char* path);

void joinPaths(char* p1, char* p2);

#endif
