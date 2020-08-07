# CS50 Lab 4
## Jordan Sanz

### common

`common` is a growing directory that contains modules that are useful to multiple pieces of the Tiny Search Engine (TSE). `common` contains the `pagedir` module, which holds functions dealing with directories that are useful to `crawler` and `indexer`, like checking to see if a directory exists and saving a page in that directory. It also contains `word` and `index`, which are two modules that are useful to `indexer` in allowing it to normalize words and load and save `indexes`.

### Usage

`common` is used as a library and is included in the make file of `crawler`, `indexer`, and the TSE. It is useful to the TSE to check parameters about directories and to save webpages into these directories. It is also used to normalize words read from webpages. It utilizes the following functions in `pagedir`:

```c
bool check_directory(const char* directory);
bool pagesaver(webpage_t *page, const char* pageDirectory, int currentCount);
bool isCrawlerDirectory(const char* pageDirectory);
```

the following functions in `index`:

```c
bool index_save(hashtable_t* hash, const char* indexFilename);
void save_item(void *arg, const char* key, void* item);
void save_counts(void *arg, const int key, int count);
void ct_delete(void *item);
hashtable_t* index_load(const char* oldIndexFilename);
```

and it uses the following function in `word`:

```c
char* normalizeWord(char* word);
```

### Implementation

`common` is implemented by utilizing the `pagedir` module, which is a simple set of two functions. `check_directory` takes a `char* pageDirectory` parameter, and `check_directory` will attempt to open it to see if it exists or not. It will return a boolean depending on the result. `pagesaver` takes a page of type *webpage_t*, a string of the *pageDirectory*, and the current unique id to save as the filename of the output. It will print the url, the max depth, and the html of that webpage into an output file with the title of the value of *currentCount*, located in *pageDirectory*.

`common` also implements functions used in `indexer`. `isCrawlerDirectory` takes a `const char* pageDirectory` and checks to see if it is a `crawler` produced directory; that is, if there are `crawler` output files in this directory. `isCrawlerDirectory` does this by trying to open a writeable file in the directory titled `.crawler`; this file should have been created by `crawler`. If the file opens successfully, the `isCrawlerDirectory` will close the file and return *true*. Otherwise, `isCrawlerDirectory` will return *false*. 

`common` also contains the `word` module, which implements the `normalizeWord` function. This function takes a *char * word* and makes all of the letters in it lowercase by looping through each letter in the string and indexing into the string. It will return the normalized word pointer. 

Further implementation concerning the `index` module can be found in IMPLEMENTATION.md in `indexer`.

### Assumptions

There are no assumptions beyond those clear from the spec.

*pageDirectory* should be of type `char *`, *page* should be of type `webpage_t`, and *currentCount* should be an `int`.

*word* should be of type `char*`. 

### Files
* `Makefile` - compilation procedure for common
* `pagedir.c` - the implementation for pagedir
* `pagedir.h` - the interface for pagedir
* `word.c` - the implementation for word
* `word.h` - the interface for word
* `index.c` - the implementation for index
* `index.h` - the interface for index

### Compilation

To compile, simply `make common.a`.