# CS50 Lab 5
## Jordan Sanz

### indexer
`indexer` is a part of the Tiny Search Engine (TSE) that takes pages of urls, depths, and html produced by `crawler` and counts the number of appearances of each word in each file. It then saves this result in an output file of its own.
`indexer` starts with a pageDirectory and an indexFilename, and as long as there are files produced by `crawler` within the pageDirectory, it will read them and add to its index storage, with it finishing with a new file of output. 

### Usage
`indexer` is a part of the TSE, and implements an `index` module made up of a `hashtable` of `countersets` that keep track of the number of words. The *key* of each `set` in the hashtable is a new word, and the *item* paired with that key is a `counter` object. The integer *key* for the counter object is then the depth of the page the word was found on, and the *count* of the counter object is the number of times that the word was seen on that page. `indexer` utilizes the following functions:

```c
int main(const int argc, const char *argv[]);
int indexer(const char* pageDirectory, const char* indexFilename);
bool readDocument(hashtable_t* hash, FILE* fp, int documentID);
char* createFilename(int documentID, const char* pageDirectory);
webpage_t* createPage(FILE* fp);
```

### Implementation

A full implementation spec is found in the IMPLEMENTATION.md file.

### Assumptions

The number of slots used to initialize the `hashtable` in indexer is 850. This was chosen because there is no way to know how many words will be found before indexing from the `crawler`-produced pages. 

The files in the `pageDirectory` should all be files produced by `crawler`; while there is a check for this, this likewise means that the files should be titled as digits, starting with 1 and incrementing by 1. 

The `pageDirectory` must be an existing directory, and the `indexFilename` should be a writeable file. 

Each `documentID` may have a maximum of 10 digits; sufficient memory will not be allocated above that number of digits.

### Files

A full list of files used is found in the IMPLEMENTATION.md spec.

### Exit Codes:
* 1: incorrect number of arguments
* 2: directory argument is not a crawler directory
* 3: index file is not openable
* 4: page cannot be created
* 5: saving index failed
* 6: allocating memory failure
* 7: failure on initializing index
* 8: problem creating filename

### Compilation

To compile, simply `make indexer.o`.

### Testing

The `testing.sh` program tests various forms of incorrect command line arguments to ensure command-line parsing and validation of those parameters works correctly. It also indexes simple sets of output pages from `crawler`, from various depths and with varying amounts of words. The output is redirected and saved to `testing.out`.
