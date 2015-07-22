CC=gcc
DBGCFLAGS=-g -Wall
CFLAGS=-O2
EXES=durbx sim0 sim1 esim sim1a
LIBS=-lm

durbx: durbx.c
	${CC} ${DBGCFLAGS} -o $@ $^

# simple state change simulator
sim0: sim0.c
	${CC} ${DBGCFLAGS} -o $@ $^ ${LIBS}

sim1: sim1.c
	${CC} ${DBGCFLAGS} -DUNPREDRA -o $@ $^ ${LIBS}

# Generate loaded dice training/unknown symbols and states
genhmmda: genhmmda.c
	${CC} ${DBGCFLAGS} -DUNPREDRA -o $@ $^ ${LIBS}

# le's look at how functions behave shall we?
esim: esim.c
	${CC} ${DBGCFLAGS} -o $@ $^ ${LIBS}

.PHONY: clean

clean:
	rm -f ${EXES} vgcore.* core*
