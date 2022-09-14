#ifndef EXECUTE_H
#define EXECUTE_H

#include "pipeline.h"
int execute(int executeInBackground, const char* cmd, int argc, const char* argv[]);

int executePipeline(int executeInBackground, PipelineJob* pipelineJob);

#endif
