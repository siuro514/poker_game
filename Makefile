#!/bin/sh

TARGET = catch_joker

#define obj files here
OBJ = catch_joker.o  

#define include files here
CC	= gcc
LIBS	= -L./ -lpoker
INCLUDES= -Ipoker_lib/

#define compile options here
CFLAGS 	= -g -Wall
DEFINE	= -DTEST

MAKE = make
MAKE_CLEAN = make clean

LIBDIRS = poker_lib

${TARGET}: ${OBJ}
	for x in $(LIBDIRS); do $(MAKE) -C $$x || exit 1 ; done
	${CC} ${CFLAGS} ${DEFINE} ${INCLUDES} -o $@ ${OBJ} ${LIBS}

clean:
	for x in $(LIBDIRS); do $(MAKE_CLEAN) -C $$x || exit 1 ; done
	rm -f *.o *.a $(TARGET)

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(DEFINE) $(INCLUDES) -c $<
