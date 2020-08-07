# Makefile for CS50 Tiny Search Engine
#
# CS 50, Summer 2020

.PHONY: all valgrind clean

############## default: make all libs and programs ##########
all: 
	make -C libcs50
	make -C common
	make -C crawler
	make -C indexer
	#make -C querier

############## valgrind all programs ##########
valgrind: all
	make -C crawler valgrind
	make -C indexer valgrind
	#make -C querier valgrind

############## clean  ##########
clean:
	rm -f *~
	make -C libcs50 clean
	make -C common clean
	make -C crawler clean
	make -C indexer clean
	#make -C querier clean
