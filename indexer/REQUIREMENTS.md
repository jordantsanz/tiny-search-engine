---
layout: page
title: TSE Indexer Requirements Spec
---

The TSE *indexer* is a standalone program that reads the document files produced by the TSE crawler, builds an index, and writes that index to a file.
Its companion, the *index tester,* loads an index file produced by the indexer and saves it to another file.

The indexer **shall**:

1. execute from a command line with usage syntax
   * `./indexer pageDirectory indexFilename`
   * where `pageDirectory` is the pathname of a directory produced by the Crawler, and
   * where `indexFilename` is the pathname of a file into which the index should be written; the indexer creates the file (if needed) and overwrites the file (if it already exists).

2. validate its command-line arguments:
	* `pageDirectory` is the pathname for an existing directory; if it cannot be verified to be a directory produced by the Crawler, the indexer shall print an error message and exit non-zero. 
	* `indexFilename` is the pathname of a writeable file; it need not already exist, but if it is not possible to open it for writing, the indexer shall print an error message and exit non-zero.

2. read documents from the `pageDirectory`, each of which has a unique document ID, wherein
   * the document `id` starts at 1 and increments by 1 for each new page,
   * and the filename is of form `pageDirectory/id`,
   * and the first line of the file is the URL,
   * and the second line of the file is the depth,
   * and the rest of the file is the page content (the HTML, unchanged).

3. build an inverted-index data structure mapping from *words* to *(documentID, count) pairs*, wherein each *count* represents the number of occurrences of the given word in the given document.
Ignore words with fewer than three characters, and "normalize" the word before indexing.
(Here, "normalize" means to convert all letters to lower-case.)
4. create a file `indexFilename` and write the index to that file, in the format described below.

The indexer *may assume* that

* `pageDirectory` has files named 1, 2, 3, ..., without gaps.
* The content of files in `pageDirectory` follow the format as defined in the specs; thus your code (to read the files) need not have extensive error checking.

The index tester **shall**:

1. execute from a command line with usage syntax
   * `./indextest oldIndexFilename newIndexFilename`
   * where `oldIndexFilename` is the name of a file produced by the indexer, and
   * where `newIndexFilename` is the name of a file into which the index should be written.
2. load the index from the `oldIndexFilename` into an inverted-index data structure.
3. create a file `newIndexFilename` and write the index to that file, in the format described below.

It need not validate its command-line arguments other than to ensure that it receives precisely two arguments; it may simply try to open the `oldIndexFilename` for reading and, later, try to open the `newIndexFilename` for writing.
You may want to run this program as part of testing script that verifies that the output is identical to (or equivalent to) the input.

The index tester *may assume* that

* The content of the index file follows the format specified below; thus your code (to recreate an index structure by reading a file) need not have extensive error checking.

### Index file format {#index-file-format}

The indexer writes the inverted index to a file, and both the index tester and the querier read the inverted index from a file; the file **shall** be in the following format.

 * one line per word, one word per line
 * each line provides the word and one or more (docID, count) pairs, in the format
	 * *word docID count [docID count]...*
 * where *word* is a string of lower-case letters,
 * where *docID* is a positive non-zero integer,
 * where *count* is a positive non-zero integer,
 * where the word and integers are separated by spaces.

Within the file, the lines may be in any order.

Within a line, the docIDs may be in any order.
