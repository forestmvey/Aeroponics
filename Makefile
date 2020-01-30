CFLAGS = -g -O2 -Wall -Wextra
CHECKFILES = src/*.c src/*.h
PERMS = -m 0444 -o root -g bin

.PHONY: all
all: bin/monitor bin/pump bin/solenoid bin/aeroponic

bin/monitor: src/monitor.c obj/common.o
	${CC} -o $@ src/monitor.c obj/common.o -lbcm2835 ${CFLAGS}

bin/aeroponic: src/aeroponic.c obj/common.o
	${CC} -o $@ src/aeroponic.c obj/common.o -lbcm2835 ${CFLAGS}

bin/pump: src/pump.c obj/common.o
	${CC} -o $@ src/pump.c obj/common.o -lbcm2835 ${CFLAGS}

bin/solenoid: src/solenoid.c obj/common.o
	${CC} -o $@ src/solenoid.c obj/common.o -lbcm2835 ${CFLAGS}

obj/common.o: src/common.c
	${CC} -c -o $@ src/common.c -lbcm2835 ${CFLAGS}

clean:
	rm -f bin/*
	rm -f obj/*

