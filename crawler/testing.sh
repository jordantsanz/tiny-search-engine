#!/bin/bash 
# testing.sh for crawler.c
#
# tests multiple error and working cases for crawler
#
# Jordan Sanz
# CS50
#

# set-up: make testing directories to hold pagesaver output
mkdir ../data
mkdir ../data/test1
mkdir ../data/test3
mkdir ../data/test4
mkdir ../data/test5
mkdir ../data/test6
mkdir ../data/test7
mkdir ../data/test8
mkdir ../data/test9

# test 0: incorrect command line arguments
echo "Testing incorrect amount of line arguments"
./crawler
./crawler one
./crawler one two

# test 1: seedURL points to non-existant server
echo "Testing when seedURL points to non-existant server"
./crawler hello.hellohellohello ../data/test1 0

# test 2: directory is non-existant
echo "Testing when directory is non-existant"
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html notdirectory 0

# test 3: maxDepth is out of bounds
echo "Testing when max depth is out of bounds"
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test3 -1
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test3 11
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test3 notnumber

# test 4: seedURL points to non-internal server
echo "Testing when seedURL points to non-internal server"
./crawler http://www.jordansanz.me ../data/test4 0

# test 5: seedURL points to valid server but non-existant page
echo "Testing crawler when seedURL points to valid server but non-existant page"
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.blah ../data/test5 0

# test 6: crawl simple, closed set of web-pages
echo "Testing crawler on simple, closed set of webpages"
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test6 0
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test6 1
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test6 2
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test6 3
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test6 4
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test6 5


# test 7: repeat tests 4 and 5 with different seed page in same site
echo "Testing crawler with same site but different seed page"
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test7 0
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test7 1
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test7 2
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test7 3
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test7 4
./crawler http://old-www.cs.dartmouth.edu/~cs50/index.html ../data/test7 5


# test 8: point crawler at wikipedia playground with depths 0, 1, 2
echo "Testing crawler with wikipedia playground with depths 0, 1, 2"
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/wikipedia/index.html ../data/test8 0
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/wikipedia/index.html ../data/test8 1
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/wikipedia/index.html ../data/test8 2

# test 9: test crawler with large depth
echo "Testing crawler with larger depth: 6, 7, 8, 9, 10"
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test9 6
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test9 7
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test9 8
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test9 9
./crawler http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html ../data/test9 10