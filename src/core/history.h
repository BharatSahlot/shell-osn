#ifndef HISTORY_H
#define HISTORY_H

#include "../globals.h"

// loads history from save file
int loadHistory(const char* saveFile);

// records command in history but doesn't save in file
int recordInHistory(const char* cmd);

// save recorded history in file
int saveHistory(const char* saveFile);

#endif
