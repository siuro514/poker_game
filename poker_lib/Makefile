#!/bin/sh

TARGET = libpoker.a

#define obj files here
OBJ = poker.o  

#define include files here
CC	= gcc
LIBS	= 
INCLUDES= -I./

#define compile options here
CFLAGS 	= -g -Wall
DEFINE	=
 
${TARGET}: ${OBJ}
	ar rv $@ $?
	mv $@ ../

clean:
	rm -f *.o *.a 

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CFLAGS) $(DEFINE) $(INCLUDES) -c $<
