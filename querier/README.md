# CS50 Lab 6
## Jordan Sanz

### querier

`querier` is a part of the Tiny Search Engine (TSE) that takes a directory of pages of urls, depths, and html produced by `crawler`, and a page of words, document IDs, and word frequencies produced by `indexer`, and then takes a query of words. It will produce the documents, if any, with the highest 'score' for that query, being the highest frequency of words for that query given, with each document having a score then ranked by score. It also supports using the keywords 'and' and 'or', with 'and' having a higher precedence than or when searching. Thus, it implements all of the functionality of the querier.

### Usage

`querier` is part of the TSE, and implements the `index` module made up of a `hashtable` of `countersets` that keep track of the scores of each document for each word. It uses another additional `counter` to keep track of the score for each document as scores for the documents based on a single word are found and combined. When `querier` finds an `or` word, the document scores of the two words (or sets of words) on the other sides of the `or` will sum, making the score for each document id a sum of those two words/word sets. If `querier` finds an `and` word, the minimum score of each document score of the words on both sides of the `and` will be taken as that document's current score. Thus, `or` is implemented as a union, and `and` is implemented as an intersection. 

To enter a query, `querier` will prompt by saying "Query?". Simply enter the words you are looking for and press the enter key. `querier` will search for that query and then prompt for another query. To finish searching for queries, press `control` + `D` keys. The rules for a query are below in the "Assumptions" section. `querier` utilizes the following functions:

```c
int main(const int argc, const char* argv[]);
int querier(const char* pageDirectory, const char* indexFilename);
int parseQuery(char* str, const char* indexFilename, const char* pageDirectory);
int separateQuery(char* str, char** words);
bool printCleanedQuery(char** words, int indicesInList);
bool calculateQuery(char** words, int indicesInList, index_t* indexModule, counters_t* orseq);
void orQuery(counters_t* andseq, counters_t* orseq);
void andQuery(counters_t* andseq, counters_t* counters_2);
void intersect_helper(void *arg, const int key, const int count);
void union_helper(void *arg, const int key, const int count);
bool orderResults(counters_t* orseq, const char* pageDirectory);
void countDocumentHelper(void *arg, const int key, const int count);
void createDocumentHelper(void *arg, const int key, const int count);
void insertionSort(void* arg, int documentCount);
int min(const int a, const int b);
bool printResults(void *arg, const char* pageDirectory, int documentCount);
char* findURL(const int key, const char* pageDirectory);
```

### Implementation

A full implementation spec is found in the IMPLEMENTATION.md file.

### Assumptions

* `pageDirectory` has files named 1, 2, 3, ..., without gaps.
* The content of files in `pageDirectory` follow the format as defined in the specs; thus your code (to read the files) need not have extensive error checking.
* The content of the file named by `indexFilename` follows our [Index file format]({{site.labs}}/lab5/REQUIREMENTS.html#index-file-format); thus your code (to recreate an index structure by reading a file) need not have extensive error checking.

While there are checks for each query, for a query to work, it should observe the following rules:

* There should only be letters in each word (i.e., it should not contain punctuation, numbers, etc)

* The words 'and' and 'or' should not come at the beginning or end of the query

* The words 'and' and 'or' should not be back-to-back in a query

* Words should be separated by at least one space

Queries can have the following structure:

* They can have a single word or a string of words, separated by spaces; `query` will interpret each word to be separated with an `and` keyword

* Words can be separated by the word `and`; this will cause an intersection of the two words, and will only return documents that have both keywords in them

* Words can be separated by the word `or`; this will cause a union of the two words, and will return documents with either of the keywords in them, but documents with both keywords in them will then have the sum of the scores of both keywords

* As a note, `and` will be interpreted before `or`. This means that `and` has precedence over `or`, and a query of "hello and goodbye or what" is the same as "what or hello and goodbye". 

### Files

A full list of files used is found in the IMPLEMENTATION.md spec.

### Exit Codes:
* 1: Wrong number of arguments provided with the program
* 2: Directory is not a crawler directory
* 3: Index file is not readable

Note: there are many possible exits with queries; however, the program will not exit, but instead, that query will end and `querier` will prompt for a new query. These are labeled with the exit code "4", but the program will not actually exit with that exit code, as `querier` will simply prompt for a new query.

### Compilation

To compile, simply `make querier.o`.

### Testing

The `testing.sh` program tests various forms of incorrect command line argumetns to ensure command-line parsing and validation of those parameters works correctly. It also tests many different queries that do not produce results and are incorrectly formatted queries. It then tests some queries that do work to test precedence and scoring, as well as ranking. It also tests queries with varying spaces in the queries. The output is redirected and saved to `testing.out`.
