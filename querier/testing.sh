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


# Test 0: incorrect number of arguments
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
 echo "j0rd@n"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "cs-50"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 2: testing conjunctions at start and end of queries
echo "Testing queries with conjunctions at start or end"
echo "and hello"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "or hello"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "hello and" |./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "hello or"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "and" ./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 3: testing double conjunctions in the middle of queries
echo "Testing queries with conjunctions hack-to-back"
echo "hello and and goodbye" | ./querier ../data/test1Crawler ../data/indexFiles/test1Index 

echo "hello or or goodbye" | ./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "hello and or goodbye" |./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "hello or and goodbye"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "Moving to successful test cases."

# Test 4: testing giving no word query
echo "Testing queries with no words"
echo ""|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 4 cont.: testing giving only spaces as query
echo "Testing giving only spaces as query"
echo "    "|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 5: testing with words not in the testfiles
echo "Testing queries with words not in the testfiles"
echo "hello"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "hello goodbye"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 6: testing single words in all cases
echo "Testing single words in all cases"
echo "dartmouth"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "Dartmouth"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "DARTMOUTH"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 7: testing two words both in the index without and with an and in between
echo "dartmouth spectrum"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "dartmouth and spectrum"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo"spectrum and dartmouth"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "spectrum dartmouth"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 8: testing two words both in the index with an or in between
echo "Testing two words with or conjunction"

echo "dartmouth or spectrum"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "spectrum or dartmouth"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 9: testing two words when one is not in index with and, or, and nothing in-between
echo "Testing with two words when one is not in index"

echo "dartmouth notaword"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "notaword dartmouth"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "dartmouth and notaword"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "notaword and dartmouth"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

echo "dartmouth or notaword"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "notaword or dartmouth"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 10: testing precedence with conjunctions
echo "Testing precedence with conjunctions"

echo "dartmouth and spectrum or canvas"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "dartmouth or spectrum and canvas"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "dartmouth and spectrum and canvas"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "dartmouth or spectrum or canvas"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 11: testing precedence when cases not found
echo "Testing precedence when cases not found"

echo "dartmouth and notaword or canvas"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "notaword and dartmouth or canvas"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "notaword or dartmouth and canvas"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "notaword and dartmouth and canvas"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "dartmouth and canvas and notaword"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "dartmouth and canvas or notaword"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 12: testing various inputs
echo "Now testing on various input strings"
echo "dartmouth and canvas and program or computer documentation or that"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "github or professors and hellothisisnotaword and computer or documentation and that"| ./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "that and github or dartmouth professors computer or documentation"|./querier ../data/test1Crawler ../data/indexFiles/test1Index

# Test 13: testing various inputs with lots of spaces everywhere
echo "Now testing with various inputs with many spaces"
echo "dartmouth       and spectrum" | ./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "      dartmouth and spectrum      "|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "dartmouth  and     spectrum"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "        dartmouth"|./querier ../data/test1Crawler ../data/indexFiles/test1Index
echo "dartmouth        "|./querier ../data/test1Crawler ../data/indexFiles/test1Index

./querier ../data/test1Crawler ../data/indexFiles/test1Index <<< EOF
echo "All tests successful."