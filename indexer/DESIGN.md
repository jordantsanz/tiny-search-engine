# TSE Indexer Design Spec

Recall the [Indexer Requirements Spec](REQUIREMENTS.md); the **indexer** indexes crawler output and determines the number of each word in each document, and prints this output into an `indexFile`. It parses an initial document from the `pageDirectory` and creates a `webpage_t` from that document, then reads the `html` of that document and puts it into a `hashtable_t` struct that is used as an index module. When all of the files have been indexed, the `hashtable_t` struct will save all of its output to a file.


### User interface

The indexer's only interface with the user is on the command-line; it must have two arguments.

```
indexer pageDirectory indexFile
```
For example:

```bash
$ indexer ../data/test1 ../data/indexFiles/output
```

### Inputs and outputs

Input: the only inputs are command-line parameters; see the User Interface above.

Output: We save all of the contents of the index `index_t` module to an output file, one word per line. 
Within an output file, we write on each line:

* a single word
* as many documentID and count pairings as exist in the index_t module

For example:

hello 1 1 2 1 
goodbye 1 1

This example would have read two documents only and found two words, hello and goodbye. Hello would have been found once in document one and once in document two, and goodbye would have been found once in document one.

### Functional decomposition into modules

We anticipate the following modules or functions:

1. *main*, which parses arguments and verifies them
2. *indexer*, which loops over crawler documents to index and initializes an index data structure
3. *readDocument*, which creates a webpage and reads the html of that webpage to find words
4. *index_save*, which saves the contents of an index to an output file
5. *indextest*, which tests the loading and saving of an index
6. *index_load*, which loads in the words and counts from an indexer output file into an index module

And some helper modules that provide data structures:
1. *index* of countersets that keep track of the counts of words in documents
2. *webpage* of html that can be created to be easily parsed for words


### Pseudo code for logic/algorithmic flow

The indexer will run as follows:

1. execute from a command line as shown in the User Interface
2. parse the command line, validate parameters, initialize other modules
3. create the first `filename` string to be explored
4. initialize the `index` data structure to be filled
5. Begin looping through documents in the crawler page directory, and for each document:
    6. Create a `webpage` for the document
    7. Parse all of the `words` longer than 3 letters from the document and normalize them
    8. Add the `word` to the `index` module with that `documentID`, or incrememnt the `count` of that `word` with that `documentID` if it already exists in the `index` module
    9. Delete the `webpage`
10. Save the contents of the `index` module by:
    11. Iterating through all of the `counters` in the index, save each `word` key to an output file, and for each `counter`:
        12. Iterating through all of the counts in each `counter` and for each count:
            13. Save the document ID and the count to the output file
14. Delete the `index` module and all of its contents
15. Return successfully

Then, for indextest:

1. Parse and validate all of the command-line arguments
2. Initialize and fill a new `index` data structure by:
    3. counting the number of lines in the `oldIndexFilename` file
    4. creating a blank index structure with that many slots
    5. while there is a `word` left in the file:
        6. create a new `counter`
        7. while there are numbers on the current line:
            8. Read in numbers, two at a time
            9. Set the `counter` to have a key as the first number (this is the `documentID`) and a `count` as the second number (this is the frequency of the `word` in that document)
        10. Insert the `counter` into the `index` and free memory for the word
    11. Return the index
12. Save the contents of the `index` module by:
    13. Iterating through all of the `counters` in the index, save each `word` key to an output file, and for each `counter`:
        14. Iterating through all of the counts in each `counter` and for each count:
            15. Save the document ID and the count to the output file
16. Delete the `index` module and all of its contents
17. Return successfully

More detailed pseudocode can be found in the IMPLEMENTATION.md file.

**normalize the word** means to convert a word into a strictly lowercase format.


### Dataflow through modules

For indexer:

1. *main* parses parameters and passes them to indexer.
2. *indexer* creates an index to track word counts inside documents; when it explores a document, it passes the index to *readDocument*, and when it wants to save the contents of the index, it passes the index to *index_save*.
3. *readDocument* parses the words from a document and keeps track of the counts of the words in `counter` structures, then inserting these into the index.
4. *index_save* outputs the contents of the indexer to the appropriate file.

For indextest:

1. *main* parses parameters and passes them to indextest.
2. *indextest* loads in a new index by calling *index_load* and passing it the oldIndexFilename string; when it saves the contents of the index, it calls *index_save*, passing it the index.
3. *index_load* parses the words and the documentID/count pairings from the oldIndexFilename file and saves them to counter objects and hashtable slots inside of the index.
4. *index_save* outputs the contents of the indexer to the appropriate file. 

### Major data structures

Three helper modules provide data structures:

1. *index* of word and documentID/count pairings
2. *hashtable* of word and counter pairings, used indirectly by *index*
3. *set* of words used indirectly by hashtable
4. *counters* of documentID and word frequencies used by *index*
5. *webpage* of URL, depth, and html

### Testing plan

0. Test various command-line errors in indexer to ensure that its command-line parsing, and the validation of those parameters, works correctly.

0. Test the indexer with no command-line arguments, not including the program itself.

0. Test the indexer with one command-line argument.

0. Test the indexer with three command-line arguments.

1. Test the indexer when the `pageDirectory` is non-existant.

2. Test the indexer when the `pageDirectory` is not a `crawler` directory.

3. Test the indexer on a small set of webpage output provided by `crawler`. 

4. Test the indextest command-line arguments with the following tests:

4. Test the indextest program with zero command-line arguments.

4. Test the indextest program with one command-line argument.

4. Test the indextest program with three command-line arguments.

5. Test the indextest program with the first parameter not being an existing file.

6. Test the indextest program on a small set of indexer output.

7. Test the indexer on a larger set of webpage output from the wikipedia url crawler output. 

8. Test the indextest on a larger output from indexer. 