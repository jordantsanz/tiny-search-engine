# CS50, Summer 2020
# Jordan Sanz

PROG = crawler
OBJS = crawler.o
LIBS = ../libcs50/libcs50-given.a ../common/common.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I../libcs50 -I../common
CC = gcc
MAKE = MAKE

$(PROG): $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@



crawler.o: ../libcs50/webpage.h ../common/pagedir.h ../libcs50/set.h ../libcs50/bag.h ../libcs50/hashtable.h ../libcs50/file.h ../libcs50/memory.h
readline.o: ../lib/readlinep.h

.PHONY: test valgrind clean

test: testing.sh
	bash -v testing.sh &> testing.out

valgrind: $(PROG)
	valgrind ./$(PROG)

clean:
	rm -rf *.dSYM 
	rm -f *~ *.o
	rm -f $(PROG)
