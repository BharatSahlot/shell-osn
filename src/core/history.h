#ifndef HISTORY_H
#define HISTORY_H

#include "../globals.h"

int initHistory();

// loads history from save file
int loadHistory();

// records command in history but doesn't save in file
int recordInHistory(const char* cmd);

// save recorded history in file
int saveHistory();

#endif
