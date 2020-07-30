# Makefile for common directory
## CS50, Jordan Sanz

OBJS = pagedir.o
LIBS = common.a
CFLAGS = -Wall -pedantic -std=c11 -ggdb

CC = gcc
MAKE = make

.PHONY: all clean

all: $(LIBS)
$(LIBS): $(OBJS)
	ar cr $(LIBS) $(OBJS)

pagedir.o: pagedir.h

clean:
	rm -f $(LIBS)
	
