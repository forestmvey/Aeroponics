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
all: bin/monitor bin/pump bin/solenoid bin/aeroponic

bin/monitor: src/monitor.c
	${CC} -o $@ src/monitor.c ${CFLAGS}

bin/aeroponic: src/aeroponic.c
	${CC} -o $@ src/monitor.c ${CFLAGS}

bin/pump: src/pump.c
	${CC} -o $@ src/pump.c -lbcm2835 ${CFLAGS}

bin/solenoid: src/solenoid.c
	${CC} -o $@ src/solenoid.c -lbcm2835 ${CFLAGS}

clean:
	rm -f bin/*

