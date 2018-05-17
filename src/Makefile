TARGET = runner
PREFIX = robot@ev3:src
GCC = ev3gcc.exe

.PHONY: all clean uninstall

all: $(TARGET)

clean:
	rm -rf $(TARGET) *.o

robot.o: robot.c robot.h
	$(GCC) -c robot.c

distmgr.o: distmgr.c params.h robot.h
	$(GCC) -c distmgr.c

params.o: params.c params.h
	$(GCC) -c params.c

mathfunc.o: mathfunc.c mathfunc.h
	$(GCC) -c mathfunc.c

$(TARGET): robot.o params.o distmgr.o mathfunc.o runner.c
	$(GCC) -o $(TARGET) runner.c robot.o params.o mathfunc.o distmgr.o -lm

simu: robot.c params.o distmgr.o mathfunc.o runner.c
	$(GCC) -DSIMU -o simu runner.c robot.c params.o mathfunc.o distmgr.o -lm

install: $(TARGET) simu
	scp $(TARGET) simu $(PREFIX)
