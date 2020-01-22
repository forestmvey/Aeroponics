CFLAGS = -g -O2 -Wall -Wextra
CPPFLAGS = -DNDEBUG
CHECKFILES = src/*.c src/*.h
PERMS = -m 0444 -o root -g bin
SRC = src/monitor.c src/pump.c src/solenoid.c src/log_writer.c src/common.c
BINTEMP = ${SRC:S/src/bin/}
BIN = ${BINTEMP:S/.c//}
OBJTEMP = ${SRC:S/src/bin/}
OBJ = ${OBJTEMP:S/.c/.o/}

.PHONY: all
all: bin/monitor bin/pump bin/solenoid bin/aeroponic

bin/monitor: src/monitor.o bin/common.o
	${CC} -o $@ src/monitor.o bin/common.o -lbcm2835 ${CFLAGS}

bin/monitor.o: src/monitor.c
	${CC} -c -o $@ src/monitor.c bin/common.o ${CFLAGS}

bin/aeroponic: src/aeroponic.o bin/common.o
	${CC} -o $@ src/aeroponic.o bin/common.o -lbcm2835 ${CFLAGS}

bin/aeroponic.o: src/aeroponic.c
	${CC} -c -o $@ src/aeroponic.c ${CFLAGS}

bin/pump: src/pump.c bin/common.o
	${CC} -o $@ src/pump.c bin/common.o -lbcm2835 ${CFLAGS}

bin/solenoid: src/solenoid.c bin/common.o
	${CC} -o $@ src/solenoid.c bin/common.o -lbcm2835 ${CFLAGS}

bin/common.o: src/common.c
	${CC} -c -o $@ src/common.c -lbcm2835 ${CFLAGS}

clean:
	rm -f bin/*

