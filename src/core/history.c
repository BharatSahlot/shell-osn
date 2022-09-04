#include "history.h"

#include <fcntl.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

int historyCount = 0;
char historyArr[HCOMMANDS_TO_STORE][MAX_CMD_LENGTH];
char tempBuf[MAX_CMD_LENGTH];

const char* historyFile;

int initHistory()
{
    static char file[MAX_PATH_SIZE];
    struct passwd* pw = getpwuid(geteuid());
    if(pw == NULL)
    {
        LogPError("history-init");
        return -1;
    }
    sprintf(file, "%s/.batak_history", pw->pw_dir);
    historyFile = file;

    struct stat st;
    if(stat(historyFile, &st) == -1)
    {
        if(creat(historyFile, S_IWUSR | S_IRUSR) == -1)
        {
            LogPError("history-init");
            return -1;
        }
    }
    return 0;
}

int loadHistory()
{
    int fd = open(historyFile, O_RDONLY);
    if(fd == -1)
    {
        LogPError("history-load");
        return -1;
    }

    char ch;
    int curCmd = 0, chIndx = 0;
    while(read(fd, &ch, 1) == 1)
    {
        if(ch == '\n')
        {
            if(chIndx == 0) continue;
            historyArr[curCmd][chIndx] = '\0';
            curCmd++;
            chIndx = 0;
            continue;
        }
        historyArr[curCmd][chIndx++] = ch;
    }
    historyCount = curCmd;
    close(fd);
    return 0;
}

int shouldRecord(const char* cmd)
{
    if(historyCount == 0) return 1;

    strcpy(tempBuf, cmd);
    char buf[MAX_CMD_LENGTH];

    char* bptr = buf;
    char* ptr = strtok(tempBuf, " \t\n");
    while(ptr != NULL)
    {
        strcpy(bptr, ptr);
        bptr += strlen(ptr);
        ptr = strtok(NULL, " \t\n");
        if(ptr != NULL)
        {
            *bptr = ' ';
            bptr++;
        }
    }
    return strcmp(historyArr[historyCount - 1], buf) != 0;
}

int recordInHistory(const char* cmd)
{
    if(!shouldRecord(cmd)) return 0;

    if(historyCount + 1 >= HCOMMANDS_TO_STORE)
    {
        for(int i = 0; i < historyCount - 1; i++)
        {
            strcpy(historyArr[i], historyArr[i + 1]);
        }
        historyCount--;
    }
    historyCount++;

    strcpy(tempBuf, cmd);
    char* historyPtr = historyArr[historyCount - 1];
    char* ptr = strtok(tempBuf, " \t\n");
    int first = 1;
    while(ptr != NULL)
    {
        if(!first) historyPtr += sprintf(historyPtr, " ");
        historyPtr += sprintf(historyPtr, "%s", ptr);
        first = 0;

        ptr = strtok(NULL, " \t\n");
    }
    return 0;
}

int saveHistory()
{
    int fd = open(historyFile, O_CREAT | O_WRONLY | O_TRUNC, S_IWUSR | S_IRUSR);
    if(fd == -1)
    {
        LogPError("history-save");
        return -1;
    }
    for(int i = 0; i < historyCount; i++)
    {
        int err = write(fd, historyArr[i], strlen(historyArr[i]));
        if(err == -1)
        {
            close(fd);
            LogPError("history-save");
            return -1;
        }

        if(historyArr[i][strlen(historyArr[i]) - 1] != '\n')
        {
            err = write(fd, "\n", 1);
            if(err == -1)
            {
                close(fd);
                LogPError("history-save");
                return -1;
            }
        }
    }
    close(fd);
    return 0;
}
