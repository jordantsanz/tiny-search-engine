# CS50 Lab 4
## Jordan Sanz

### common

`common` is a growing directory that contains modules that are useful to multiple pieces of the Tiny Search Engine (TSE). Currently, `common` just contains the `pagedir` module, which holds functions dealing with directories that are useful to `crawler`, like checking to see if a directory exists and saving a page in that directory. 

### Usage

`common` is used as a library and is included in the make file of `crawler` and the TSE. It is useful to the TSE to check parameters about directories and to save webpages into these directories. It utilizes the following functions in `pagedir`:

```c
bool check_directory(const char* directory);
bool pagesaver(webpage_t *page, const char* pageDirectory, int currentCount);
```

### Implementation

`common` is implemented by utilizing the `pagedir` module, which is a simple set of two functions. `check_directory` takes a `char* pageDirectory` parameter, and `check_directory` will attempt to open it to see if it exists or not. It will return a boolean depending on the result. `pagesaver` takes a page of type *webpage_t*, a string of the *pageDirectory*, and the current unique id to save as the filename of the output. It will print the url, the max depth, and the html of that webpage into an output file with the title of the value of *currentCount*, located in *pageDirectory*.

### Assumptions

There are no assumptions beyond those clear from the spec.

*pageDirectory* should be of type `char *`, *page* should be of type `webpage_t`, and *currentCount* should be an `int`. 

### Files
* `Makefile` - compilation procedure
* `pagedir.c` - the implementaiton
* `pagedir.h` - the interface

### Compilation

To compile, simply `make common.a`.