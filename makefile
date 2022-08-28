objs = main.o logger.o utils.o cmds/cd.o cmds/pwd.o cmds/echo.o

all : $(objs)
	gcc -O2 -o build/build $(objs)

.PHONY : all

main.o: logger.h utils.h
logger.o : logger.h
utils.o : utils.h
cmds/cd.o: logger.h
cmds/pwd.o: logger.h
cmds/echo.o: logger.h

clean :
	rm -r build/build $(objs)
