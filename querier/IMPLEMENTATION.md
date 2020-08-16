# IMPLEMENTATION.md
## Jordan Sanz

### querier implementation

`querier` implements many different functions. The main pseudocode for the entire module is as follows:
* check and verify all of the parameters
* prompt for a query
* split all of the words in the query and validate that they are all allowed
* validate that the ordering of the words in the query is allowed
* print the query
* calculate the overall scores of each document relevant to the query
* rank the results of the query
* print the ranked results of the query
* prompt for another query until querier is ended by the user

The pseudocode here will be expanded upon below. Following is a complete list of the function prototypes implemented in `querier`:

```c
int main(const int argc, const char* argv[]);
bool isCrawlerDirectory(const char* pageDirectory);
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

Following is a description of the pseudocode in `querier`, in order by function, with the interface of each described and any error handling described as well.

### Functions

#### int main(const int argc, const char* argv[])

`main` is the first function of the querier module; it serves to check the passed parameters. It takes two parameters, `const int argc` and `const char* argv[]`, which represent the count and different parameters passed through the command line. There should be 3 command line arguments: the program (./querier), `const char* pageDirectory`, and `const char* indexFilename`. The pseudocode for main is as follows:

* check that there are 3 arguments
* check that `pageDirectory` is a crawler-directory by calling `isCrawlerDirectory`
* check that `indexFilename` is openable and writeable
* close `indexFilename`
* call `querier`, passing in `pageDirectory` and `indexFilename`
* return with error code 0

The error handling for main is as follows:

* If there are not 3 arguments, exit with error code 1
* If the directory is not a crawler directory, exit with error code 2
* If `indexFilename` is not openable or readable, exit with error code 3

#### bool isCrawlerDirectory(const char* pageDirectory)
`isCrawlerDirectory` serves to check that `pageDirectory` is a directory that was used by `crawler` by checking that the file `.crawler` exists in `pageDirectory`. It is in the `pagedir.c` program, and it returns a boolean: if `pageDirectory` is a `crawler` directory, it returns *true*; otherwise, it returns *false*. `isCrawlerDirectory` takes a singular argument: `const char* pageDirectory`. The pseudocode is as follows:

* allocate memory for the path of the file to check
* create the filename string of the path of the file to check
* attempt to open the file
* if it opens and is writeable, close the file
* free the memory for the pathname string
* return true

The error handling is as follows:

* if the file is not openable or it is not writeable or both, return false;

#### int querier(const char* pageDirectory, const char* indexFilename)

`querier` is a function that serves to prompt the user for queries. It takes two parameters, `const char* pageDirectory` and `const char* indexFilename`, passed through `main`. It will prompt for a query, and then call *parseQuery* to start analyzing the query. Its pseudocode is as follows:

* Create a variable `char* str` to hold the user's query
* Prompt the user for a query and wait until a query is entered
* Once the user enters a query, ending it by pressing the enter key, saves the query string to `str`
* Passes `str`, `indexFilename`, and `pageDirectory` to *parseQuery*
* frees `str`
* prints a divider and prompts for another query
* Once an EOF is enterd by the user, returns successfully

There is no error handling in the `querier` function.

#### int parseQuery(char* str, const char* indexFilename, const char* pageDirectory)

`parseQuery` serves to parse a given query string and calculate and print its results by calling various functions. It takes three parameters: `char* str` being the input query string, `const char* indexFilename` being the file path of the indexer-produced file, and `const char* pageDirectory` being the directory of the crawler-produced documents. Its pseudocode is as follows:

* create a counters object `orseq` by calling `counters_new()` from the `counters` module
* load in the `index_t` index module from the `indexFilename` by calling `index_load` from the `index` module in `common`
* Check to make sure the index module loaded correctly and counter was created correctly
* Create an array of character pointers and check to make sure memory was allocated correctly
* fill the array of words and find the number of indices in the array by calling *separateQuery*
* check to make sure there were words entered in the query, and if not:
    * free the array of words
    * delete the index by calling `index_delete` from the `index` module
    * delete `orseq` by calling `counters_delete` from the `counters` module
    * return with error code 4
* print the cleaned query and check for any errors in printing
* calculate the results of the query by calling *calculateQuery* and check for errors
* order the results and print them by calling *orderResults*
* delete `orseq` and `indexModule` with their respective delete functions, free the array of words and return successfully


Error handling for `parseQuery` is as follows:

* if the counter object could not be created, return with error code 4
* if the index module could not be created, return with error code 4
* if the array of character pointers could not be created, exit
* if there were no words entered in the query, return with error code 4
* if printCleanedQuery fails, free and return
* if there were errors in calculating the results of the query, free and return

#### int separateQuery(char* str, char** words)
`separateQuery` serves to separate all of the words given in the input query and put them in the array of words. It takes two parameters: `char* str` of the input string and `char** words` being the pointer to an array of word pointers, currently empty. Its pseudocode is the following:

* check to see if no query was given
* create `int index1` to keep track of the front of a word and set to 0
* create `int index2` to keep track of the back of a word
* while `index1` is on a space, move it forward
* check to see if `index1` is now on a `\0`
* set `index2` as `index1`'s value
* create `int endval` to be `strlen(str) - 1`
* while `endval` is on a space or is on `\0`, move it backwards in `str`
* once it is on a non-space and non-`\0`, move it forward one space; this is the last important character in the input string
* if `str[endval]` is currently a space, set it as `\0` instead
* create `int wordPositionInList` to keep track of the number of indices in the list, and set as -1
* while `index2` is not equal to `endval`:
    * if `str[index2]` is on a `\0`, break out of the loop
    * if `str[index2]` is not on a space, move it forward one index and continue
    * if `index1` equals `index2`, move `index2` forward one index and continue
    * if `str[index1]` is not a letter, move `index1` forward one index and continue
    * if `index1` is greater or equal to `index2`, move `index2` forward one index and continue
    * if `index1` is less than `index2`, `str[index1]` is a letter, and `str[index2]` is a space:
        * set `str[index2]` as `\0`
        * increment `wordPositionInList` by 1
        * set `words[wordPositionInList]` to be a pointer to `str[index1]`, thus keeping track of that word
        * while `index1` is not equal to `index2`, move `index1` forward
        * if `index2` is not at the end of the input string:
            * move index 1 and 2 both forward
* After the loop, while `index1` is on a space, move it forward
* if `str[index1]` is a letter, increment `wordPositionInList` by one and create a pointer to `str[index1]` at `words[wordPositionInList]`
* return `wordPositionInList`


`separateQuery` has the following error handling:

* if the length of `str` is 0, no query was given, and return with -1
* if `index1` is initially set to `\0` from moving it off of spaces, only spaces were given in the query, and return with -1

#### bool printCleanedQuery(char** words, int indicesInList)
`printCleanedQuery` serves to validate every word in the array of words, normalize every word, and print the entire query without any extra spaces. It takes two parameters: `char** words` and `int indicesInList`. It has the following pseudocode:

* for every index in the the `words` array:
    * normalize the word at that index by calling `normalizeWord` from the `words` module in `common`
    * check to make sure the word was normalized correctly
    * set that word at `words[index]`
    * if the current index being checked is the first or the last index
        * check if the word at that index is "and" or "or"
* print every word in the array of words
* print a new line and return true


It has the following error handling:
* if the word failed to normalize, print an error message and return false
* if the word at the first or last index is `and` or `or`, print error message and return false


#### void normalizeWord(char* word)
`normalizeWord` serves to normalize every word passed into it by changing all of the letters to lowercase. It is in the `word.c` program and it takes a single argument of `char* word`. It return the `char* word` in all lowercase letters. Its pseudocode is as follows:

* for every letter in `word`:
    * call `tolower()` on that letter
* return word

The error handling is the following:
* if a character of the word being checked is not a letter, return *NULL*

#### bool calculateQuery(char** words, int indicesInList, index_t* indexModule, counters_t* orseq)
`calculateQuery` serves to calculate the scores of every document based on the words in the given query. It takes four paramters: `char** words` being the array of words in the query, `int indicesInList` being the number of indices filled in the words array, `index_t* indexModule` being the index module loaded, and `counters_t* orseq` being the main counters_t object holding the scores of the documents, but is currently empty. It has the following pseudocode:

* create `char* conjunction` and set it to *NULL*
* create `bool madeBlankAnd` and set it to *false*
* create `counters_t* andseq` by trying to find the `counters_t` object associated with the first word in `words` in the `indexModule` by calling `index_find` from the `index` module
* check to see if a counter was found; if not, create a blank new counter by calling `counters_new` from the `counters` module, set it to `andseq`, and set `madeBlankAnd` to true; this is because this counter will need to be freed since `index_delete` will not free it
* create `counters_t* counters_2`
* for every index in the array of words that is filled:
    * if the current word in `words` is "and":
        * check to see if there is already a conjunction
        * set `conjunction` as "and" and continue
    * if the current word in `words` is "or":
        * check to see if there is already a conjunction set
        * set `conjunction` as "or" and continue
    * if `andseq` is *NULL*:
        * search for the current word in `indexModule` with `index_find` and set it as `andseq`
        * if nothing was found, make `andseq` a blank, new counter and set `madeBlankAnd` to be *true*
        * reset `conjunction` to *NULL*; this is because this case will only happen directly after an `or` conjunction in the query, and if you wait to set it to *NULL*, you can first check to make sure the current word is not *and*
        * continue
    * if `conjunction` is *NULL* or `conjunction` is *and*:
        * if `counters_2` is *NULL*:
            * make a blank `counters` and set it as `counters_2`
            * perform an `andQuery` and pass in `andseq` and `counters_2`
            * call `counters_delete` on `counters_2` from the `counters` module
            * set `conjunction` and `counters_2` to *NULL* and continue
        * else, perform an `andQuery` and pass in `andseq` and `counters_2`
        * set `counters_2` and `conjunction` to *NULL*
    * if `conjunction` is *or*
        * perform an `orQuery` by passing in `andseq` and `orseq`
        * if `madeBlankAnd` is *true*
            * free `andseq`
        * set `andseq` to *NULL* and continue
* check if `orseq` is *NULL*
* if `andseq` is *NULL*, return *true*
* else:
    * perform an `orQuery` by passing in `andseq` and `orseq`
    * if `madeBlankAnd` is *true*, delete `andseq` and set `madeBlankAnd` to *false*
    * return *true*


It has the following error handling:
* If there is already a conjunction when setting "and" or "or", print an error message, free the blank `andseq` counter if necessary, and return *false*
* if `orseq` is *NULL*, print an error message and return false
* if creating a new counters object results in it being *NULL*, print error message and return *NULL*

#### void orQuery(counters_t* andseq, counters_t* orseq)

`orQuery` serves to perform an `orQuery` calculation for `calculateQuery`. It takes two counters, `counters_t* andseq` and `counters_t* orseq` as parameters, and it sums the scores of all of the documents inside of them. It has the following pseudocode:

* check to make sure both `andseq` and `orseq` are valid
* create a `struct twocts args` holding `orseq` and `andseq`
* call `counters_iterate` passing in `andseq`, `&args`, and `union_helper` as the helper function

Error handling:

* if `andseq` or `orseq` is *NULL*, print error message and exit

#### void andQuery (counters_t* andseq, counters_t* counters_2)

`andQuery` serves to perform an `andquery` calculation for `calculateQuery` by taking two counters objects, `counters_t* andseq` and `counters_t* counters_2` as parameters, and finding the documents that exist in both, and setting the minimum score from both counters as the score for that document in `andseq`. It has the following pseudocode:

* check that `andseq` and `orseq` are valid
* create `struct twocts args` to hold `andseq` and `orseq`
* call `counters_iterate` and pass in `andseq`, `&args`, and `intersect_helper` as the helper function


Error handling:
* if `andseq` or `orseq` is *NULL*, print error message and exit

#### void intersect_helper(void *arg, const int key, const int count)

`intersect_helper` simply performs an intersection between two counters for an `andQuery`. It takes three parameters: `void* arg`, `const int key`, and `const int count`. It has the following pseudocode:

* set `struct twocts *two` to `arg`
* call `counters_set` on `two->result`, at the given `key`, with the value of the `min` of the given `count` and `counters_get` of `two->another` at the given `key`

It has no error handling.

#### void union_helper(void *arg, const int key, const int count)

`union_helper` serves to perform a union between the counts of two counters for an `orQuery`. It takes three parameters: `void *arg`, `const int key`, and `const int count`. It has the following pseudocode:

* set `struct twocts *two` to be `arg`
* create `int sum` to be `count` added to the return of `counters_get` on `two->result` with the given `key`
* call `counters_set` on `two->result` with the given `key` and the value of `sum`

There is no error handling.

#### int min(const int a, const int b)

`min` simply finds the minimum value between its two parameters `const int a` and `const int b` and returns that value. There is no error handling.

#### bool orderResults(counters_t* orseq, const char* pageDirectory)

`orderResults` serves to create an array of `documentResult_t` objects out of all of the documents in `orseq`, order them by score, and print them. It takes two parameters: `counters_t* orseq` which is now filled, and `const char* pageDirectory`. It has the following pseudocode:

* create `int documentCount` and set it to 0
* call `counters_iterate` on `orseq` passing in `&documentCount` and `countDocumentHelper` as the helper function
* check if `documentCount` is 0
* create `documentResult_t** results`
* allocate memory for `results`
* set each index of `results` to *NULL*
* call `counters_iterate` on `orseq`, passing in the empty array `results` and `createDocumentHelper` as the helper function
* call `insertionSort` on the filled array, passing in the array and `documentCount`
* call `printResults`, passing in `results`, `pageDirectory`, and `documentCount`
* for every document in `results`, free it
* free `results`
* return *true*


It has the following error handling:
* if `documentCount` is 0, print that there were no documents found, and return
* if memory could not be allocated for `results`, print error and exit

#### void countDocumentHelper(void *arg, const int key, const int count)

`countDocumentHelper` serves to count the number of documents in `orseq`. It takes three parameters: `void *arg`, `const int key`, and `const int count`. It has the following pseudocode:

* set `int *nitems` as `arg`
* if `nitems` is not *NULL*, increment it by 1

It has no error handling other than checking to first make sure `nitems` is not *NULL*.

#### void createDocumentHelper(void *arg, const int key, const int count)

`createDocumentHelper` serves to create a `documentResult_t` document out of each document and score, and add it to `results`. It takes three parameters: `void* arg`, `const int key`, and `const int count`. It has the following pseudocode:

* set `documentResult_t** results` as `arg`
* create `documentResult_t* document` and allocate memory for it
* set `document->docID` as `key`
* set `document->count` as `count`
* create `int i` as 0, count the current number of documents in `results` and increment `i` by one for each document, and insert the `document` at that `i` index in `results`

Error handling:
* if memory could not be allocated for `documentResult_t* document`, print error and exit

#### void insertionSort(void *arg, int n)

`insertionSort` serves to sort all of the `documentResult_t` documents in `results`. It takes two arguments: `void* arg` and `int n`. It has the following pseudocode:

* set `documentResult_t** results` as `arg`
* create `int i, j`
* create `documentResult_t* key`
* for `i=1`, and while `i < n`, adding one to `i` each time:
    * set `key` as `results[i]`
    * set `j` as `i - 1`
    * while `j >= 0` and `results[j]->count < key->count`
        * set `results[j + 1]` as `results[j]`
        * set `j` as `j - 1`
    * set `results[j + 1]` as `key`

This has no error handling.

#### bool printResults(void* arg, const char* pageDirectory)

`printResults` serves to print the sorted array of `documentResults` to standard output. It takes 3 parameters: `void *arg`, `const char* pageDirectory`, and `int documentCount`. It has the following pseudocode:

* set `documentResult_t** results` as `arg`
* set `int bad` as 0
* for `int index = 0` is less than `documentCount`:
    * if `results[index]->count` is 0
        * increment `bad` by 1
* set `documentCount` to be `documentCount - bad`
* check if `documentCount` is 0
* print the matching results output title, with `documentCount` being the number of results
* for every `documentResult` in `results`
    * if `results[index]->count` is not 0, which it shouldn't be, but as a final check
        * find the URL for that document by calling `findURL` and passing in `results[index]->docID` and `pageDirectory`
        * print the `results[index]->count`, `results[index]->docID`, and url returned by `findURL`
        * free the `url`
* return *true*


Error handling:

* if `documentCount` is 0, print that no documents were found, and return *false*

#### char* findURL(const int docID, const char* pageDirectory)

`findURL` serves to find the `url` for each `documentResult_t document` in `results` by finding the correct document in `pageDirectory`, reading the first line of the document, and returning the string of the first line, which will be the URL. It takes two parameters: `const int docID` and `const char* pageDirectory`. It has the following pseudocode:

* create a `FILE* fp` file pointer
* allocate memory for `char* str` and `char* filename`
* use `strcpy` to copy the `pageDirectory` string onto `filename`
* put a `/` at the end of `filename`
* save the string version of `docID` onto `str`
* put `str` at the end of `filename`
* open `filename` for reading and check that it opened correctly
* read the first line from the `fp` file and save it to `char* url`
*  free `filename`, `str`, and close `fp`
* return the `url`


It has the following error handling:

* if memory cannot be allocated for `str` or `filename`, print error and exit
* if `filename` couldn't be opened, print error message and return *NULL*

### Data Structures

#### index_t

The `index_t` data structure is essentially a `hashtable` of `sets` of `counters`, with added functionality coming from the functions `index_load` and `index_save`. It holds a word in every slot in the `hashtable`, and for each word, a `counter` is assoicated with it. This `counter` holds documentID and count pairings to represent the frequency of a word in any given document. To create an `index` module, simply call `index_new` and add in the word/counter pairings. Additionally, to create an `index` module, `index_load` can also be called as long as an index file has been saved by `index_save`. To delete an `index` module, `index_delete` must be called while also passing `ct_delete` as the item helper function. `index` has the functions `index_new`, `index_insert`, `index_delete`, `index_find`, and `index_iterate` that are all simply calling their `hashtable` versions.

#### documentResult_t

`documentResult_t` is a `struct` object that is utliized in `querier` to hold a document's ID and score for any given query. It holds `int docID` and `int count`, being the document ID and the score, respectively. It is used when iterating through `orseq` to find the documents that have scores for a given query; it is useful because it allows for easy storage and sorting in an array.

#### hashtable_t

`hashtable_t` is a `struct` object that holds two components: *int num_slots* and *struct table*, where *struct table* holds a set of *set_t* items. *int num_slots* determines how many slots are in the table; in this case, it is the number of lines in the `indexFile` provided by the user. `hashtable` is used in the `index` module to store all of the `words` and `counterset` pairs. It is implemented as a set of `sets`, holding all of the words as *keys* and their respective `counters` as the *value*. 

#### counters_t

`counters_t` is a `struct` object that is utilized by `index_t` and is also utilized by `querier`. In an `index` module, it is used to store a document ID along with the frequency of a certain word in that document. It holds an `const int key` and a `const int count`. In `querier`, it is used to keep track of the `score` for each document with any given query. It is used in `andQuery` and `orQuery` to merge scores based off of an intersection or union, and `counters_iterate` is often used to iterate through all of the documents found for that given query. It can be deleted with `counters_delete` if it is a `counter` made by `querier`, and it will be deleted by `index_delete` if it is a `counter` in the `index` module. 

#### set_t

`set_t` is a `struct` object that is utilized by `hashtable`. It holds one component: `struct setnode *head`, which is a `struct` object that holds a `char* key`, `void* item`, and `setnode* next`. `set_t` is implemented as a linked list, with each `setnode` pointing to the next `setnode`. While it isn't explicitly used in `querier`, `hashtable` uses `set` by having a set of `sets` in its table; this allows for the `hashtable` to hold many `keys` but also not hold any duplicates, since `set` does not allow duplicate keys. 

#### twocts

`twocts` is a `struct` object that is utilized by `querier` to compare two `counters_t` objects in the `counters_iterate` function. It has two components: a `counters_t* result` and `counters_t* another`. The `result` will be a counter that is changed and updated, and `another` will be a counter that is deleted. `twocts` is utilized when performing an `andQuery` or an `orQuery`. If it is an `andQuery`, it holds the `andseq` counter as the `result` counter and `counters_2` and the `another` counter. `counters_iterate` will find the minimum score for a given documentID in both counters, and set the `result` counter in `twocts` to that score. In an `orQuery`, `twocts` is utilized by allowing `counters_iterate` to sum every document score in `result` with the document score in `another`, saving all of the sums in `result`. No memory is allocated for the `struct twocts` itself, but rather, memory is allocated for each `counters_t` object it holds. Thus, it need not be freed.

### Files

* `Makefile` - compilation procedure
* `.gitignore` - list of ignored files
* `DESIGN.md` - design specs
* `fuzzquery.c` - extra file for testing
* `querier.c` - the implementation for querier
* `README.md` - readme file
* `REQUIREMENTS.md` - requirements for implementation
* `testing.out` - output file for testing
* `testing.sh`- testing program
* `common.a` - testing library
    * `pagedir.h` - interface for directory functions
    * `pagedir.c` - implementation for directory fucntions
    * `index.c` - implementation for index module
    * `index.h` - interface for index module
    * `word.c` - implementation for word functions
    * `word.h` - inteface for word functions
    * `Makefile` - compilation procedure for common library
* `libcs50.a` - library for cs50 module in TSE
    * `hashtable.h` - interface of hashtable
    * `jhash.c` - implementation of hashing function
    * `jhash.h` - interface of hashing function
    * `Makefile` - compilation procedure for library
    * `memory.c` - implementation of memory functions
    * `set.h` - interface of set
    * `set.c` - implementation of set
    * `webpage.h` - interface for webpage functions
    * `webpage.c` - implementation for webpage functions
    * `file.h` - interface for file functions
    * `file.c` - implementation for file functions
