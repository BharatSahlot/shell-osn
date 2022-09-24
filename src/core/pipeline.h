#ifndef PIPELINE_H
#define PIPELINE_H

#include "../globals.h"
#include <sys/types.h>

typedef struct PipelineJob PipelineJob;

struct PipelineJob
{
    int argc;

    // array of pointers to locations in cmd, is NULL terminated
    const char* args[MAX_ARGS];

    // full cmd as it is typed with null terminators in between
    char cmd[MAX_CMD_LENGTH];

    const char* inputFile;
    const char* outputFile;
    int appendOutput;

    pid_t pid;

    int in;
    int fd[2];

    PipelineJob* prev;
    PipelineJob* next;
};

PipelineJob* parsePipeline(char* cmd);
void cleanPipeline(PipelineJob* root);

#endif
