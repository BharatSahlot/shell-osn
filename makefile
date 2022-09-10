CC := gcc
CFLAGS := -O2 -Wall

objects := src/main.o src/utils.o src/logger.o \
		   src/builtins/builtins.o src/builtins/cd.o src/builtins/pwd.o \
		   src/builtins/ls.o src/builtins/echo.o src/builtins/pinfo.o \
		   src/builtins/history.o src/builtins/discover.o \
		   src/builtins/jobs.o src/builtins/sig.o src/builtins/fg.o \
		   src/core/execute.o src/core/parse.o src/core/prompt.o \
		   src/core/history.o src/core/process_list.o \
		   src/core/autocomplete.o src/core/io.o \

all: $(objects) src/globals.h
	$(CC) $(CFLAGS) $(objects) -o batak

$(objects): %.o: %.c

.PHONY : all

clean:
	rm -f $(objects) batak
