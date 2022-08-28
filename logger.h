#ifndef LOGGER_H
#define LOGGER_H

#define LOGL_INFO 0
#define LOGL_WARN 1
#define LOGL_ERROR 2

void Log(int level, const char* format, ...);

#endif
