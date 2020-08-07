# Makefile for common directory
## CS50, Jordan Sanz

OBJS = pagedir.o index.o word.o
LIBS = common.a
CFLAGS = -Wall -pedantic -std=c11 -ggdb

CC = gcc
MAKE = make

.PHONY: all clean

all: $(LIBS)
$(LIBS): $(OBJS)
	ar cr $(LIBS) $(OBJS)

pagedir.o: pagedir.h
index.o: index.h
word.o: word.h

clean:
	rm -f $(LIBS)
	rm -f *.o
	
