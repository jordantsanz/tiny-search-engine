# CS50, Summer 2020 - indexer Makefile
# Jordan Sanz
# Received help on making the Makefile from Wylie Kasai

C = ../common
L = ../libcs50

PROG1 = indexer
PROG2 = indextest
LLIBS = $L/libcs50.a $C/common.a
OBJS1 = indexer.o
OBJS2 = indextest.o

CC = gcc
MAKE = make
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C

.PHONY: all test clean

all: $(PROG1) $(PROG2)

$(PROG1): $(OBJS1) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@

$(PROG2): $(OBJS2) $(LLIBS)
	$(CC) $(CFLAGS) $^ $(LLIBS) -o $@

indexer.o: pagedir.o word.o index.o
indexer.o: webpage.o memory.o bag.o hashtable.o counters.o file.o
indextest.o: pagedir.o word.o index.o
indextest.o: webpage.o memory.o bag.o hashtable.o counters.o file.o
pagedir.o: $C/pagedir.h
pagedir.o: webpage.o file.o
word.o: $C/word.h
index.o: $C/index.h
index.o: counters.o hashtable.o file.o
webpage.o: $L/webpage.h
file.o: $L/file.h
counters.o: $L/counters.h
hashtable.o: $L/hashtable.h
bag.o: $L/bag.h
memory.o: $L/memory.h

test: testing.sh
	bash -v testing.sh &> testing.out
	

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG1)
	rm -f $(PROG2)
	rm -f core

