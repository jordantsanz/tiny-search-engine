# IMPLEMENTATION.md
## Jordan Sanz

### indexer implementation

`indexer` implements many different functions. The main pseudocode for the entire module is as follows:
* check and verify all of the parameters
* create first filename template
* initialize new hashtable
* read each document in the directory provided
* save the output of the index structure into a file

The pseudocode here will be expanded upon below. Following is a complete list of the function prototypes utilized in `indexer`:


```c
int main(const int argc, const char* argv[]);
bool isCrawlerDirectory(const char* pageDirectory);
int indexer(const char* pageDirectory, const char* indexFilename);
char* createFilename(int documentID, const char* pageDirectory);
bool readDocument(index_t* index, FILE* fp, int documentID);
webpage_t* createPage(FILE* fp);
char* normalizeWord(char* word);
bool index_save(index_t* index, const char* indexFilename);
void save_item(void *arg, const char* key, void *item);
void save_counts(void* arg, const int key, int count);
void ct_delete(void* item);
```

Following is a description of the pseudocode in `indexer`, in order by function, with an the interface of each described and any error handling described as well.


### Functions

#### int main(const int argc, const char* argv[])

`Main` is the first argument of the indexer module; it serves to check the passed parameters. It takes two parameters, `const int argc` and `const char* argv[]`, which represent the count and different parameters passed through the command line. There should be 3 command line arguments: the program (./indexer), `const char* pagedirectory`, and `const char* indexFilename`. The pseudocode for main is as follows:

* check that there are 3 arguments
* check that `pageDirectory` is a crawler-directory by calling `isCrawlerDirectory`
* check that `indexFilename` is openable and writeable
* close `indexFilename`
* call `indexer`, passing in `pageDirectory` and `indexFilename`
* return with the return code given by `indexer`

The error handling for main is as follows: 

* If there are not 3 arguments, exit with error code 1
* If the directory is not a crawler directory, exit with error code 2
* If `indexFilename` is not openable or writeable, exit with error code 3

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


#### int indexer(const char* pageDirectory, const char* indexFilename)

`indexer` is a function that serves to create an index module, loop through all of the documents in a directory and read them all, then save the contents of the index module to a new file. It takes two arguments: `const char* pageDirectory` and `const char* indexFilename`. The pseudocode for the function is as follows:

* create the first filename "`pageDirectory`/1" by calling `createFilename`
* check to make sure the filename was created correctly
* initialize an index to serve as the index module
* check to make sure the index was created correctly
* while the current file is openable and readable: 
    * attempt to read the document
    * free the filename of the document
    * create the filename of the next document by calling `createFilename`
    * check to make sure the filename was created correctly
* free the filename of the last document
* attempt to save the contents of the index module to an output file by calling `index_save`
* return successfully

Error handling for `indexer` is as follows:

* if the filename is not created correctly, return with error code 8
* if the index is not created correctly, return with error code 7
* if a document is not able to be read, print an error message, but continue to the next file
* if the index module cannot be saved, return with error code 5

#### char* createFilename(int documentID, const char* pageDirectory)

`createFilename` serves to create the string of the path to the readable `crawler` output file for indexer. It returns a `char * filename` that is used by indexer. It takes two parameters: `int documentID` and `const char* pageDirectory`. It allocates memory for each filename that then must be freed in indexer. The pseudocode for `createFilename` is as follows:

* allocate memory for the document id; this is assumed to be no longer than 10 digits long, and the memory allocated reflects this assumption
* allocate memory for the filename string
* check to make sure memory was allocated correctly
* copy the `pageDirectory` string onto the `filename` string with `strcpy`
* Add a slash to the end of the `filename` string
* Add the string version of the `documentID` to the `filename` string
* free the allocated memory for the `documentID` string (but not the `filename` string)
* return the `filename` string

The error handling for `createFilename` is as follows: 

* if memory was not allocated correctly for the `documentID` or the `filename`, return NULL

#### bool readDocument(hashtable_t* hash, FILE* fp, int documentID)

`readDocument` serves to read all of the words in any given `crawler` document. This function takes three parameters: the main index module `index_t* index`, `FILE* fp` to the document, and the current `documentID`. It returns a boolean: if it reads the entire document successfully, it returns *true*; otherwise, it returns *false*. The pseudocode of `readDocument` is as follows:

* create a `webpage_t* page` by calling `createPage`
* check to make sure `page` was created correctly
* initialize a `pos` position counter and a `char* word` variable
* while there is still a word that can be retrieved from the webpage:
    * check to make sure the word retrieved is greater than or equal to 3 letters long
    * normalize the word by calling `normalizeWord`
    * attempt to put the word in the index module
        * if successful, create a new counter by calling `counters_new`
        * add the current `documentID` to the `counter`
        * insert the word and the counter into the index module
        * free the word and continue
    * else:
        * retreive the counter from the index module associated with the current `word`
        * free the current `word`
        * increment the counter by 1
* after this, delete the `webpage`
* close the document
* return *true*

The error handling of `readDocument` is as follows:

* If `page` was not created correctly, then return *false*
* if the word retrieved is less than 3 letters long, free the word and continue to the next word
* if unable to increment the counter by 1, return *false*

#### webpage_t* createPage(FILE* fp)

`createPage` serves to create a `webpage_t` data structure. It takes one argument: `FILE* fp` to the current document produced by `crawler`. It returns a `webpage_t*` that has the url, depth, and html of the `crawler` produced document. The pseudocode of `createPage` is as follows:

* read the first line of the document to retrieve the url
* read the second line of the document to retrieve the depth
* read the remainder of the document to retrieve the html
* check to make sure all of these were retrieved successfully
* convert the depth string to an `int` and free the depth string
* create a new `webpage_t*` by calling `webpage_new` from `webpage.h` module
* check to make sure the `webpage` was created successfully
* return the page

The error handling of `createPage` is as follows:

* if any of the url, depth, or html could not be retrieved from the page, return NULL
* if the page could not be created successfully by `webpage_new`, return NULL

#### char* normalizeWord(char* word)

`normalizeWord` serves to normalize every word passed into it by changing all of the letters to lowercase. It is in the `word.c` program and it takes a single argument of `char* word`. It return the `char* word` in all lowercase letters. Its pseudocode is as follows:

* for every letter in `word`:
    * call `tolower()` on that letter
* return word

There is no error handling in this function.


#### bool index_save(index_t* index, const char* indexFilename)

`index_save` serves to save all of the contents of the `index` module. It is found in the `index.c` program module. It is called by `indexer` and it takes two arguments: `index_t* index` being the index data structure, and `const char* indexFilename` being the output file location. It returns a boolean: if it saves the contents of the `index` module successfully, it returns true; otherwise, it returns false. The pseudocode of `index_save` is as follows:

* check to make sure the index module exists
* open the output file
* iterate through the index module by calling `index_iterate` and passing in `save_item` as the item function
* delete the index module, passing in `ct_delete` as the item deleting function
* close the file
* return *true*

The error handling of `index_save` is as follows:

* If the index module does not exist, return *false*

#### save_item(void* arg, const char* key, void* item)

`save_item` is a function used by `index_iterate` to save the counter items to an output file. It takes three parameters: `void* arg` which is the filepointer of the output file, `const char* key` being the word in that slot of the index module, and `void* item` which is a counter item. It returns nothing. Its pseudocode is as follows:

* cast `arg` as type `FILE*`
* cast `item` as type `counters_t*`
* print the word
* call `counters_iterate`, passing the `counter`, `arg`, and `save_counts` as the item helper function
* print a new line

It has no error handling.


#### save_counts(void* arg, const int key, int count)

`save_counts` serves as a helper function for `counters_iterate` to print the contents of a `counter` to an output file. It takes three parameters: `void* arg` as the file pointer for the output file, `const int key` as the document ID, and `int count` as the count of the counter at that ID. It returns nothing. The psuedocode of `save_counts` is as follows:

* cast `arg` as type `FILE*`
* print the `documentID` and `count`

It has no error handling.

#### void ct_delete(void* item)

`ct_delete` serves as a helper function for `hashtable_delete` in deleting the index module. It takes one parameter: `void* item` being the counter object. It returns nothing. The pseudocode is as follows:

* cast `item` as type `counters_t*`
* if the counters item is not null, delete it by calling `counters_delete`

This has no error handling.


### indextest

`indextest` is another program in the `index` directory that runs a test on `indexer` by loading the file produced by `indexer` and attempting to reproduce it identically. The pseudocode of indextest is as follows:

* Check command line arguments
* load the target index file contents into the index data structure
* create a new indexFile and write the contents of the index data structure to the file
* return successfully

The pseudocode here will be expanded on further shortly. The functions involved in `indextest` are as follows:

```c
int main(const int argc, const char* argv[]);
int indextest(const char* oldIndexFilename, const char* indexFilename);
index_t* index_load(const char* oldIndexFilename);
```

`indextest` also utilizes `index_save`, `save_item`, `save_counts`, and `ct_delete` in the exact same way as `indexer`.

#### int main(const int argc, const char* argv[])

`main` serves as the first function for `indextest` and checks to make sure all of the parameters are correct. It takes two parameters: `const int argc` being the number of command-line arguments, and `const char* argv[]` being the actual command-line arguments. There should be three command line arguments: the program (./indextest), `const char* oldIndexFilename` and `const char* indexFilename`. The pseudocode for `main` is as follows:

* check to make sure there are 3 arguments
* check to make sure the `oldIndexFilename` is readable
* check to make sure the `indexFilename` is openable and writeable
* close both the old and new files
* call `indextest`, passing the `oldIndexFilename` and `indexFilename`
* return with the return code given by `indextest`

The error handling for `main` is as follows:

* if there are not exactly 3 arguments, return with error code 1
* if the `oldIndexFilename` cannot be opened, return with error code 2
* if the `indexFilename` cannot be opened and written in, return with error code 3

#### int indextest(FILE* fpold, const char* indexFilename)

`indextest` serves to call `indexload` and `indexsave`. It takes two parameters: `const char* oldIndexFilename` and `const char* indexFilename`. It returns the correct exit code. Its pseudocode is as follows:

* create a new `index_t*` index module by calling `index_load` and passing `oldIndexFilename`
* check to make sure the index module was created correctly
* attempt to save the contents of the index module
* return successfully with exit code 0

The error handling of `indextest` is as follows:

* if memory was not allocated correctly for the index module, return with error code 4
* if the index module cannot be saved correctly, exit with error code 5


#### index_t* index_load(FILE* fpold)

`index_load` is a function described in `index.c` that loads in an index data structure from a file produced by `indexer`. It takes one parameter: `const char* oldIndexFilename` being the filename of the file produced by `indexer`. It returns a `index_t*` index module that is filled with the contents of the `oldIndexFilename`. The pseudocode of `index_load` is as follows:

* open the `oldIndexFilename` file
* find the number of slots to initialize the `index` module with by counting the lines in `oldIndexFilename` with the function `lines_in_file` from `file.h`
* create the `index` module by calling `index_new` from `index.h`
* create word, id, and count variables
* while there is a word in the file still:
    * read the current word in
    * create a new `counter`
    * while there are still numbers in the current row:
        * read the numbers and set the counter at that id to the count read-in, with the id being the first number read-in and the count being the second number read-in
    * add the counter to the index module with that associated word
    * free the word string
* close the file
* return the filled `index` module

The error handling is as follows:

* if memory is unable to be allocated for the `index` module or the `counter`, return *NULL*

### Data structures

#### index_t

The `index_t` data structure is essentially a `hashtable` of `sets` of `counters`, with added functionality coming from the functions `index_load` and `index_save`. It holds a word in every slot in the `hashtable`, and for each word, a `counter` is assoicated with it. This `counter` holds documentID and count pairings to represent the frequency of a word in any given document. To create an `index` module, simply call `index_new` and add in the word/counter pairings. Additionally, to create an `index` module, `index_load` can also be called as long as an index file has been saved by `index_save`. To delete an `index` module, `index_delete` must be called while also passing `ct_delete` as the item helper function. `index` has the functions `index_new`, `index_insert`, `index_delete`, `index_find`, and `index_iterate` that are all simply calling their `hashtable` versions.

#### webpage_t

`webpage_t` is a `struct` object that has three components: *char * url*, *int depth*, and *char * html*. It is defined in `webpage.h`, and more information about its implementation can be found there. The `webpage` module is used to store urls, depth, and html altogether in order to be saved in output files and searched for more urls. It exists in two states: with an uninitialzed html, where the html is `NULL`; and with initialized html, after the `webpage` has been fetched with `webpage_fetch` from the `webpage.h` module. A `webpage` can be made using `webpage_new` and deleted using `webpage_delete`. In `indexer`, a `webpage` is created by reading the crawler output pages and reading the url, depth, and html into a `webpage_new`. This can then be used to read through each word with `webpage_getNextWord`. 

#### hashtable_t

`hashtable_t` is a `struct` object that holds two components: *int num_slots*, and *struct table*, where *struct table* holds a set of *set_t* items. *int num_slots* determines how many slots are in the table; in this case, there are 850 slots in the `hashtable` in `indexer`, and for `indextest`, the number of slots is equal to the number of lines in the old indexer file. The *struct table* is then implemented as a set of sets, holding all of the urls that have already been searched. `hashtable_insert` is used to insert into the table, and since `hashtable` does not allow duplicates, this lets `indexer` know if a word already exists in the table. `hashtable_new` is used to create a `hashtable` and `hashtable_delete` is used to delete a `hashtable`, with `ct_delete` being passed to it to delete the counter objects as items, in this case.

#### set_t

`set_t` is a `struct` object that is utilized by `hashtable`. It holds one component: `struct setnode *head`, which is a `struct` object that holds a `char* key`, `void* item`, and `setnode* next`. `set_t` is implemented as a linked list, with each `setnode` pointing to the next `setnode`. While it isn't explicitly used in `indexer`, `hashtable` uses `set` by having a set of `sets` in its table; this allows for the `hashtable` to hold many `keys` but also not hold any duplicates, since `set` does not allow duplicate keys. 


### Files

* `Makefile` - compilation procedure
* `.gitignore` - list of ignored files
* `indexer.c` - the implementation
* `indextest.c` - the implementation for the testing
* `testing.sh` - unit testing file
* `testing.out` - output file for testing
* `DESIGN.md` - design specs
* `REQUIREMENTS.md` - requirements for implementation
* `common.a` - library for common functions in TSE
    * `pagedir.h` - interface for directory functions
    * `pagedir.c` - implementation for directory functions
    * `index.c` - implementation for index module
    * `index.h` - interface for index module
    * `word.c` - implementation for word functions
    * `word.h` - interface for word functions
    * `Makefile` - compilation procedure for common library
* `libcs50.a` - library for cs50 module in TSE
    * `hashtable.h` - interface of hashtable
    * `jhash.c` - implementation of hashing function
    * `jhash.h` - interface of hashing function
    * `Makefile` - compilation procedure for library
    * `memory.c` - implementation of memory functions
    * `memory.h` - interface for memory functions
    * `set.h` - interface of set
    * `set.c` - implementation of set
    * `webpage.h` - interface for webpage function
    * `webpage.c` - implementation of webpage functions
    * `file.h` - interface for file functions
    * `file.c` - implementation for file functions