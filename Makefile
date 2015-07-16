CC=gcc
DBGCFLAGS=-g -Wall
CFLAGS=-O2
EXES=durbx

durbx: durbx.c
	${CC} ${DBGCFLAGS} -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXES} vgcore.* core*
