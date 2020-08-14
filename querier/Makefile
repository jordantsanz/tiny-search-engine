# Makefile for querier
# CS50, Summer 2020
# Jordan Sanz

L = ../libcs50
C = ../common

PROG = querier
OBJS = $L/memory.o $L/webpage.o $L/jhash.o $L/file.o $L/bag.o $C/pagedir.o $C/index.o $C/word.o
LIBS = $L/libcs50-given.a $C/common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L -I$C
CC = gcc
MAKE = MAKE

all: $(PROG)

$(PROG): querier.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@


.PHONY: all clean

test: testing.sh
	bash -v testing.sh &> testing.out

valgrind: $(PROG)
	valgrind ./$(PROG)

clean:
	rm -rf *.dSYM 
	rm -f *~ *.o
	rm -f $(PROG)