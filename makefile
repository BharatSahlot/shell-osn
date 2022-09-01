CC := gcc
CFLAGS := -O2

objects := src/main.o src/utils.o src/logger.o \
		   src/builtins/builtins.o src/builtins/cd.o src/builtins/pwd.o \
		   src/builtins/ls.o src/builtins/echo.o src/builtins/pinfo.o \
		   src/builtins/history.o \
		   src/core/execute.o src/core/parse.o src/core/prompt.o \
		   src/core/history.o

$(objects): %.o: %.c

all: $(objects) src/globals.h
	$(CC) $(CFLAGS) $(objects) -o batak

.PHONY : all

clean:
	rm -f $(objects) batak
