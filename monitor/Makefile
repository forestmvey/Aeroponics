CFLAGS = -g -O2 -Wall -Wextra
CPPFLAGS = -DNDEBUG
CHECKFILES = src/*.c src/*.h
PERMS = -m 0444 -o root -g bin
SRC = src/monitor.c src/pump.c src/solenoid.c
BINTEMP = ${SRC:S/src/bin/}
BIN = ${BINTEMP:S/.c//}
OBJTEMP = ${SRC:S/src/bin/}
OBJ = ${OBJTEMP:S/.c/.o/}

.PHONY: all
all: bin/monitor bin/pump bin/solonoid bin/aeroponic

bin/monitor: src/monitor.c
	${CC} -o $@ src/monitor.c ${CFLAGS}

bin/aeroponic: src/aeroponic.c
	${CC} -o $@ src/aeroponic.c ${CFLAGS}

bin/pump: src/pump.c
	${CC} -o $@ src/pump.c -lbcm2835 ${CFLAGS}

bin/solonoid: src/solonoid.c
	${CC} -o $@ src/solonoid.c -lbcm2835 ${CFLAGS}

clean:
	rm -f bin/*

