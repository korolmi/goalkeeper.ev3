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

mathfuncs.o: mathfuncs.c mathfuncs.h
	$(GCC) -c mathfuncs.c

$(TARGET): robot.o params.o distmgr.o mathfuncs.o runner.c
	$(GCC) -o $(TARGET) runner.c robot.o params.o mathfuncs.o distmgr.o

install: $(TARGET)
	scp $(TARGET) $(PREFIX)
