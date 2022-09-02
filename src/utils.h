#ifndef UTILS_H
#define UTILS_H

#include <pwd.h>
#include <grp.h>

// returns username if successfull else NULL
const char* getUsernameFromId(uid_t uid);

// returns groupname if successfull else NULL
const char* getGroupnameFromId(gid_t uid);

// returns the current working dir or NULL
const char* getWorkingDir();

// adds ~ to the path beginning if HOME is a prefix of it
const char* addTildaToPath(const char* path);

// makes the path absolute
const char* makePathAbsolute(const char* path);

// joins two paths which dont contain ., .. and ~
void joinPaths(char* p1, const char* p2);

#endif
