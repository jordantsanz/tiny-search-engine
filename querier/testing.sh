#!/bin/bash
# testing.sh - test program for querier
# 
# CS50, Summer 2020
#
# Jordan Sanz

# Set-up for testing
mkdir ../data/test1Crawler
mkdir ../data/test7

../crawler/crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test1Crawler 1
../indexer/indexer ../data/test1Crawler ../data/indexFiles/test1Index

../crawler/crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test7 3
../indexer/indexer ../data/test7 ../data/indexFiles/test7Index


# Test 0: correct number of arguments
echo "Testing with wrong number of command line arguments"
./querier
./querier one
./querier one two three

# Test 0 cont.: directory is not crawler directory
echo "Testing when directory is not crawler directory"
./querier notCrawlerDirectory indexFile

# Test 0 cont.: indexFile is not real file
echo "Testing when indexFile is not real file"
./querier test1 notRealFile

# Test 1: testing non-word queries
echo "Testing queries that have non-words"
./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"j0rd@n"

./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"cs-50"

# Test 2: testing conjunctions at start and end of queries
echo "Testing queries with conjunctions at start or end"
./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"and hello"

./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"or hello"

./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"hello and"

./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"hello or"

./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"and"

# Test 3: testing double conjunctions in the middle of queries
echo "Testing queries with conjunctions hack-to-back"
./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"hello and and goodbye"

./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"hello or or goodbye"

./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"hello and or goodbye"

./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"hello or and goodbye"

# Test 4: testing giving no word query
./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<""

# Test 5: testing with words not in the testfiles
./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"hello"

./querier ../data/test1Crawler ../data/indexFiles/test1Index 1<"hello goodbye"
