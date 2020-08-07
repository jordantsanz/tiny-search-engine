#!/bin/bash
#
# testing.sh - test file for indexer.c
# 
#
#
# Jordan Sanz
# CS50, 2020

# set-up: make testing directories

mkdir ../data
mkdir ../data/test1
mkdir ../data/test2
mkdir ../data/test3
mkdir ../data/test8
mkdir ../data/test9
mkdir ../data/notCrawlerDirectory
mkdir ..data/indexFiles

../crawler/crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test1 0
../crawler/crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test2 1
../crawler/crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test3 2
../crawler/crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/wikipedia/index.html ../data/test8 1
../crawler/crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test9 10

# test 0: incorrect command line arguments
echo "Testing incorrect amount of line arguments"
./indexer
./indexer one
./indexer one two three

# test 1: directory is non existant
echo "Testing when directory is non-existant"
./indexer notReal ../data/indexFiles/indexFilename

# test 2: testing when directory is not a crawler directory
echo "Testing when directory is not a crawler directory"
./indexer notCrawlerDirectory ../data/indexFiles/indexFilename

# test 3: testing on small set of webpage output
echo "Testing on a small set of crawler webpage output"
./indexer ../data/test1 ../data/indexFiles/test1File
./indexer ../data/test2 ../data/indexFiles/test2File
./indexer ../data/test3 ../data/indexFiles/test3File

# test 4: indextest incorrect commandline arguments
echo "Indextest has incorrect number of commandline arguments"
./indextest
./indextest one
./indextest one two three

# test 5: indextest first file is not a file
echo "Indextest first parameter is not a file"
./indextest notAfile newFile

# test 6: indextest on a small set of webpage output
echo "Indextest tested on a small set of indexer output with diff command to check"
./indextest ../data/indexFiles/test1File ../data/indexFiles/test1FileCopy

echo "Running diff:"
diff -q ../data/indexFiles/test1File ../data/indexFiles/test1FileCopy

./indextest ../data/indexFiles/test2File ../data/indexFiles/test2FileCopy
diff -q ../data/indexFiles/test2File ../data/indexFiles/test2FileCopy

./indextest ../data/indexFiles/test3File ../data/indexFiles/test3FileCopy
diff -q ../data/indexFiles/test3File ../data/indexFiles/test3FileCopy

# test 7: indexer on larger set of webpage output
echo "Testing indexer on larger output"

./indexer ../data/test8 ../data/indexFiles/test8
./indexer ../data/test9 ../data/indexFiles/test9

# test 8: indextest on larger set of webpage output
echo "Testing indextest on larger output"

./indextest ../data/indexFiles/test8 ../data/indexFiles/test8Copy
./indextest ../data/indexFiles/test9 ../data/indexFiles/test9Copy


echo "Testing completed."
