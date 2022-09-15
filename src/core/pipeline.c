#include "pipeline.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void cleanPipeline(PipelineJob* root)
{
    PipelineJob* job = root;
    while(job->next != NULL)
    {
        if(job->prev != NULL)
        {
            if(job->fd[0] != -1) close(job->fd[0]);
            if(job->fd[1] != -1) close(job->fd[1]);

            free(job->prev);
            job->prev = NULL;
        }
        job = job->next;
    }
    if(job->prev != NULL)
    {
        if(job->fd[0] != -1) close(job->fd[0]);
        if(job->fd[1] != -1) close(job->fd[1]);
        free(job->prev);
        job->prev = NULL;
    }
    free(job);
}

PipelineJob* makePipelineJob()
{
    PipelineJob* job = (PipelineJob*) malloc(sizeof(PipelineJob));
    job->next = job->prev = NULL;
    job->inputFile = job->outputFile = NULL;
    job->pid = -1;
    job->argc = job->appendOutput = 0;
    job->fd[0] = job->fd[1] = job->in = -1;
    memset(job->cmd, 0, sizeof(job->cmd));
    memset(job->args, 0, sizeof(job->args));
    return job;
}

int parseJob(PipelineJob* job)
{
    char* savePtr = NULL;
    char* str = strtok_r(job->cmd, " \t\n", &savePtr);
    while(str != NULL)
    {
        job->args[job->argc++] = str;
        str = strtok_r(NULL, " \t\n", &savePtr);
    }
    job->args[job->argc] = NULL;

    int newArgc = job->argc;

    int outputMode = 0; // 0 -> stdout, 1 -> file, 2 -> append
    const char* outFile = NULL;

    int inputMode = 0; // 0 -> stdout, 1 -> file
    const char* inFile = NULL;
    for(int i = 0; i < job->argc; i++)
    {
        if(strcmp(">", job->args[i]) == 0)
        {
            if(i == 0)
            {
                Log(LOGL_ERROR, "parse error: empty command\n");
                return -1;
            }
            if(outputMode != 0)
            {
                Log(LOGL_ERROR, "parse error: found > or >> more than once\n");
                return -1;
            }
            outFile = job->args[i + 1];
            outputMode = 1;
            i++;
            if(newArgc == -1) newArgc = i;
        } else if(strcmp(">>", job->args[i]) == 0)
        {
            if(i == 0)
            {
                Log(LOGL_ERROR, "parse error: empty command\n");
                return -1;
            }
            if(outputMode != 0)
            {
                Log(LOGL_ERROR, "parse error: found > or >> more than once\n");
                return -1;
            }
            outFile = job->args[i + 1];
            outputMode = 2;
            i++;
            if(newArgc == -1) newArgc = i;
        } else if(strcmp("<", job->args[i]) == 0)
        {
            if(i == 0)
            {
                Log(LOGL_ERROR, "parse error: empty command\n");
                return -1;
            }
            if(inputMode != 0)
            {
                Log(LOGL_ERROR, "parse error: found < more than once\n");
                return -1;
            }
            inFile = job->args[i + 1];
            inputMode = 1;
            i++;
            if(newArgc == -1) newArgc = i;
        } else
        {
            if(inputMode != 0 || outputMode != 0)
            {
                Log(LOGL_ERROR, "parse error: extra strings after redirecting input/output\n");
                return -1;
            }
        }
    }
    job->outputFile = outFile;
    job->inputFile = inFile;
    job->appendOutput = outputMode == 2;
    job->argc = newArgc;
    return 0;
}

PipelineJob* parsePipeline(char *cmd)
{
    PipelineJob* root = NULL;
    PipelineJob* cur = NULL, *prev = NULL;

    char* savePtr = NULL;
    char* str = strtok_r(cmd, "|", &savePtr);
    while(str != NULL)
    {
        cur = makePipelineJob();
        if(root == NULL) root = cur;

        strcpy(cur->cmd, str);
        if(prev) prev->next = cur;
        cur->prev = prev;
        cur->next = NULL;

        if(parseJob(cur) < 0)
        {
            while(root != NULL)
            {
                cur = root->next;
                free(root);
                root = cur;
            }
            return NULL;
        }

        if(cur->prev != NULL && cur->inputFile != NULL)
        {
            Log(LOGL_ERROR, "parse error: process output can only be pipelined or redirected\n");
            while(root != NULL)
            {
                cur = root->next;
                free(root);
                root = cur;
            }
            return NULL;
        }

        if(cur->outputFile != NULL)
        {
            // to check if its the last command, output redirection for last command should work
            str = strtok_r(NULL, "|", &savePtr);
            if(str == NULL) continue;

            Log(LOGL_ERROR, "parse error: process output can only be pipelined or redirected\n");
            while(root != NULL)
            {
                cur = root->next;
                free(root);
                root = cur;
            }
            return NULL;
        }

        prev = cur;
        str = strtok_r(NULL, "|", &savePtr);
    }
    return root;
}
