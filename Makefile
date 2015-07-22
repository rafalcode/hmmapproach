CC=gcc
DBGCFLAGS=-g -Wall
CFLAGS=-O2
EXES=durbx sim0
LIBS=-lm

durbx: durbx.c
	${CC} ${DBGCFLAGS} -o $@ $^

# simple state change simulator
sim0: sim0.c
	${CC} ${DBGCFLAGS} -o $@ $^ ${LIBS}

.PHONY: clean

clean:
	rm -f ${EXES} vgcore.* core*
