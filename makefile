objs = main.o logger.o utils.o cmds/cd.o cmds/pwd.o cmds/echo.o cmds/ls.o

all : $(objs)
	gcc -O2 -o build/build $(objs)

debug : $(objs)
	gcc -g -o build/build $(objs)

.PHONY : all

main.o: logger.h utils.h cmds/cmds.h
logger.o : logger.h
utils.o : utils.h logger.h
cmds/cd.o: logger.h cmds/cmds.h utils.h
cmds/pwd.o: logger.h cmds/cmds.h
cmds/echo.o: cmds/cmds.h
cmds/ls.o: logger.h cmds/cmds.h utils.h

clean :
	rm -r build/build $(objs)
