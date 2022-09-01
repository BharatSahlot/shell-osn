#include "history.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int historyCount = 0;
char historyArr[MAX_CMD_LENGTH][HCOMMANDS_TO_STORE];

int loadHistory(const char *saveFile)
{
    int fd = open(saveFile, O_RDONLY);
    if(fd == -1)
    {
        LogPError("history");
        return -1;
    }

    char ch;
    int curCmd = 0, chIndx = 0;
    while(read(fd, &ch, 1) == 1)
    {
        if(ch == '\n')
        {
            historyArr[curCmd][chIndx] = '\0';
            curCmd++;
            chIndx = 0;
            continue;
        }
        historyArr[curCmd][chIndx++] = ch;
    }
    close(fd);
    return 0;
}

int recordInHistory(const char *cmd)
{
    // no need to save if exact duplicate of previous command
    if(strcmp(cmd, historyArr[historyCount - 1]) == 0) return 0;

    if(historyCount + 1 >= HCOMMANDS_TO_STORE)
    {
        for(int i = 0; i < historyCount - 1; i++)
        {
            strcpy(historyArr[i], historyArr[i + 1]);
        }
        historyCount--;
    }
    strcpy(historyArr[historyCount++], cmd);

    return 0;
}

int saveHistory(const char *saveFile)
{
    int fd = open(saveFile, O_WRONLY | O_TRUNC | O_CREAT);
    if(fd == -1)
    {
        LogPError("history");
        return -1;
    }
    for(int i = 0; i < historyCount; i++)
    {
        int err = write(fd, historyArr[i], strlen(historyArr[i]));
        if(err == -1)
        {
            LogPError("history");
            return -1;
        }
        if(historyArr[i][strlen(historyArr[i]) - 1] != '\n')
        {
            err = write(fd, "\n", 1);
            if(err == -1)
            {
                LogPError("history");
                return -1;
            }
        }
    }
    close(fd);
    return 0;
}
