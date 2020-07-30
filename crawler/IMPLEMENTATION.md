# IMPLEMENTATION.md
## Jordan Sanz

### crawler implementation

`crawler` implements many different functions. The main pseudocode for the entire module is as follows:
* check and verify all of the parameters
* beginning with the seedURL and continuing with every url after until the maximum depth is reached:
  * fetch the html for the url's webpage
  * save the url, depth, html into an output file
  * scan the page for the next url to repeat the process with

The pseudocode here will be expanded upon below. Following is a complete list of the function prototypes utilized in `crawler`:

```c
int main(const int argc, const char* argv[]);
bool check_directory(const char* directory);
int crawler(char* seedURL, const char* pageDirectory, const int maxDepth);
webpage_t* initialize_webpage(char* url, int depth, char* html);
bool pagefetcher(webpage_t *page);
bool pagesaver(webpage_t *page, const char* pageDirectory);
bool pagescanner(webpage_t *page, int currentDepth, bag_t *searchBag, hashtable_t *urlHash);
void input_delete(void* item);
void null_delete(void* item);
```

Following is a description of the pseudocode in `crawler`, in order by function, with an the interface of each described and any error handling described as well.

### Functions

#### int main(const int argc, const char* argv[])

`main` takes two arguments: `const int argc`, which is the number of arguments of input on the command line, and `const char* argv[]` is the array of arguments entered. `main` is the first function that `crawler` utilizes, and serves to check parameters to make sure they are suitible for `crawler`. There should be four input arguments passed to main: the execution of the program (./crawler), the seedURL, the pageDirectory, and the maxDepth. The pseudocode of `main` is as follows:

* check to make sure that there are four input arguments: the program execution, the seedURL, the pageDirectory, and the maxDepth
* allocate memory for the seedURL
* check to make sure the seedURL is internal
* check to make sure the directory exists with check_directory
* check to make sure maxDepth is a number and is greater than or equal to 0 and less than or equal to 10
* call `crawler`
* exit

Error handling for `main` includes the following:
* checking to make sure there are four arguments; if not, exit with error code 1
* checking to make sure the seedURL is internal; if not, exit with error code 2
* utilizing check_directory to check that the pageDirectory exists; if not, exit with error code 3
* check that maxDepth is a number; if not, exit with error code 4
* check that maxDepth is greater than or equal to 0 and less than or equal to 10; if not exit with error code 4

#### bool check_directory(const char* directory)

`main` makes a call to `check_directory` to ensure that the *pageDirectory* that has been input into `crawler` exists. `check_directory` takes one parameter: a *const char* * directory. This should be a directory that exists, and it will be the directory that output is saved into. The pseudocode for `check_directory` is as follows:

* allocate memory for a pathname to a non-existant file
* attempt to write a file called `.crawler` in the passed-in directory
* if the file cannot be written, throw an error and return false
* if the file can be written, close the file, free the pathname memory, and return true

Error handling for `check_directory` is as follows:

* if the file cannot be written into the directory, `check_directory` will immediately return false, causing a return from `crawler` and an exit from the program


#### int crawler(char* seedURL, const char* pageDirectory, const int maxDepth)

`crawler` calls the functions and performs actions that allow the program to find, write, and find more webpages with the Tiny Search Engine. This function implements a `bag` and a `hashtable` to keep track of pages and URLs. The `bag` holds webpages that have yet to be visited, while the `hashtable` holds URLs as keys that have already been visited, with a blank string as the item associated with it. `Bag` is implemented as a linked-list `struct`, and `hashtable` is implemented as a set of `sets`, which are implemented as linked-list `structs`.

`crawler` takes three parameters: *char *seedURL*, *const char * pageDirectory*, and *const int maxDepth*. These are passed from `main`, and have already been checked and verified by `main` to be as expected. `crawler` will use these values to begin a loop of webpage fetching, saving, and scanning. The pseudocode for `crawler` is as follows:

* initialize the first webpage of type *webpage_t* by calling `initialize_webpage`, passing in the *seedURL*, 0 as the current depth, and NULL as the html
  * verify that the webpage was indeed created
* initialize the `bag` struct by calling `bag_new`
  * verify that the `bag` struct was indeed created
* insert the first page into the bag
* initialize the `hashtable` module by calling `hashtable_new`
  * verify that the `hashtable` was successfully created
* insert the seedURL into the `hashtable`
* initialize the variable *currentCount* to keep track of how many files have been written so far
* begin a loop of extracting pages from the bag with `bag_extract`, stopping only when all pages from the bag have been extracted; within this loop, for each page extracted:
  * increase the *currentCount* by one
  * fetch its html by calling `pagefetcher`
    * if the html cannot be fetched, delete the page by calling `webpage_delete` and move onto the next page from the `bag` 
  * save the page to an output file by calling `pagesaver`
    * if pagesaver fails, return exit code 9 and exit
  * if the maximum depth is greater than the current `webpage_t` page's depth, scan for more urls on the current page
  * delete the current page by calling `webpage_delete`
* after every `webpage_t` webpage is extracted from the `bag`, delete the `bag` and the `hashtable`
* return successfully

Error handling for `crawler` is as follows:

* if `initialize_webpage` fails to correctly allocate memory and create the webpage, `crawler` will exit with error code 5;
* if the `bag` struct is unable to be created correctly, `crawler` will exit with error code 6;
* if the `hashtable` struct is unable to be created correctly, `crawler` will exit with error code 7;
* if the `webpage_t` page cannot have its html fetched correctly, `crawler` will delete it and move onto the next `webpage_t` page
* if the `webpage_t` page cannot be written into an output file, `crawler` will return with exit code 9


#### webpage_t* initialize_webpage(char* url, int depth, char* html)

`initialize_webpage` is a function used to create a `webpage_t` struct data module. This data module is defined in `webpage.h`, and is imported for use in `crawler`. `initialize_webpage` takes three parameters: *char * url*, *int depth*, and *char * html*. The *char * url*  is the url pointing to that page, the *int depth* is how deep that page is in reference to the initial seed page, and the *char * html* is the html on that page. When a page is initialized, it does not have html yet, as that is found in `pagefetcher`; thus, `NULL` is passed in as the html for `initialize_webpage`. `initialize_webpage` calls `webpage_new` from the `webpage.h` module, which allocates memory for it; thus, every `webpage_t` data structure must be freed by `crawler`. The pseudocode for `initialize_webpage` is as follows:

* initialize a new `webpage_t` webpage by calling `webpage_new` from the `webpage.h` module, passing in the *url*, *depth*, and *html* parameters from `initialize_webpage`
* check to make sure the webpage was made correctly
* return the created webpage

Error handling for `initialize_webpage` is as follows:

* if `webpage_new` returns `NULL` for the webpage, `intialize_webpage` will return `NULL`, causing `crawler` to return and exit

#### bool pagefetcher(webpage_t *page)

`pagefetcher` is a function that retrieves the html for a `webpage_t` webpage and sets it as that `webpage_t` webpage's html. It does this by taking a *webpage_t * page* as a parameter; this page will be a page that currently has its html set to `NULL`. `pagefetcher` works by calling `webpage_fetch` from the `webpage.h` module, and then checks to make sure the html was retrieved successfully. The pseudocode for `pagefetcher` is as follows:

* make a call to `webpage_fetch`, storing the result as a `bool`
* check to make sure that the webpage was initialized successfully
* return true if so; return false otherwise

Error handling for `pagefetcher` is as follows:

* If `webpage_fetch` returns false, `pagefetcher` will return false, causing `crawler` to continue to the next page in the bag

#### bool pagesaver(webpage_t *page, const char* pageDirectory, int currentCount)

`pagesaver` is a function that saves a `webpage_t` webpage's contents to a unique output file in the `pageDirectory`. It takes three parameters: *webpage_t * page*, *const char * pageDirectory*, and *int currentCount*. The `webpage_t` page will be a page that has html added to it already. *pageDirectory* will be the location that the page output file should be saved, and *currentCount* will be the title of that output file. `pagesaver` writes output files with the first line of the file being the page's URL, the second line being the page's depth, and the remainder of the file being the page's html. `pagesaver` does this by normalizing the url and then retrieving all three components of the given `webpage` by calling `webpage_getURL`, `webpage_getDepth` and `webpage_getHTML` from the `webpage.h` module. 

`pagesaver` must also build the `filename` for the page to be saved at, putting the directory in front. It does this by creating two blank strings: one for the size of the number being passed in as the title of the output file, and one for the directory name. It then concatenates these strings together with `strcat` from the `string.h` module after they have had the *pageDirectory* and the *currentCount* copied to them with `strcpy` from the `string.h` module, with a slash in-betweeen.  The pseudocode for `pagesaver` is as follows: 

* initialize a filepointer
* allocate memory for the blank string representing the filename and for the blank string representing the file directory
* copy the *pageDirectory* onto the blank string representing the file directory, and concatenate this string with a slash, utilizing `strcpy` and `strcat`, respectively
* copy the value of *currentCount* onto the other string variable as a string using `sprintf` from the standard library
* concatenate the filename string with the directory string using `strcat`
* begin writing the file at this location
* check to make sure the file has been opened correctly
* normalize the url of the page by calling `NormalizeURL` from the `webpage.h` module
* write the url, depth, and html of the page onto lines one, two, and three and on, respectively, in the output file
* close the file
* free the filepath string and the filename string

Error handling for `pagesaver` is as follows:

* if the new file cannot be written, throw an error using `assertp`

#### bool pagescanner(webpage_t *page, int currentDepth, bag_t *searchBag, hashtable_t *urlHash)

`pagescanner` is a function that searches a webpage for more URLs that can then be searched by `crawler`. `pagescanner` takes four parameters: *webpage_t * page*, *int currentDepth*, *bag_t * searchBag*, and *hashtable_t * urlHash*. The *page* is a pointer to a `webpage_t` page that has already been written into an output file, and is now about to be searched for more URLs. The *currentDepth* is the depth of the page passed into `pagescanner`. *searchBag* is the `bag` of pages that have yet to be searched, and *urlHash* is the `hashtable` of URLs that have already been searched.

`pagescanner` works in a loop by calling `webpage_getNextURL` from the `webpage.h` module to find every URL on the current page. This function allocates memory for each URL, so this memory will be freed later on in `crawler`. For each URL, `pagescanner` normalizes the url by calling `NormalizeURL` from the `webpage.h` module, and then checks to see if the url is internal with the `isInternalURL` function from the `webpage.h` module. If it is internal, it will check to see if it has already been searched by attempting to insert into the `hashtable` with the url as the key and a blank string as the item. If it has not been searched yet, it will create a new page for the url by calling `initialize_webpage` and passing in the found url, *currentDepth + 1*, and NULL for the html. It will then add this new page to the *searchBag*. The pseudocode for `pagescanner` is the following:

* create a variable to keep track of the position on the page, initialized at 0
* create a variable to hold the found url
* start a while loop that runs until `webpage_getURLNext` does not return a url; for each url found, do the following:
  * normalize the url using `NormalizeURL` from the `webpage.h` module
  * check to make sure the url is internal with the `IsInternalURL` function from the `webpage.h` module
  * check to make sure the url hasn't already been searched by attempting to insert it into the `hashtable` with `hashtable_insert`
  * create a new `webpage_t` page with the found url as its url, *currentDepth + 1* as its depth, and `NULL` as its html
  * add the `webpage_t` page to *searchBag*
* after every url on the page has been checked, `pagescanner` will always return true

Error handling for `pagescanner` is as follows:

* if the url found is not internal, it will free the memory allocated for the url and continue to the next url found on the page
* if the url cannot be inserted into the `hashtable`, then the memory allocated for the url will be freed and `pagescanner` will continue to the next found url on the page

#### void input_delete(void *item)

`input_delete` is a simple helper function to delete the leftover pages in the bag. There shouldn't be any pages left in the bag when `bag_delete` is called, but this function acts as a placeholder and a backup in case there is any error. `input_delete` takes one parameter: *void *item*, which is the item that needs to be deleted from the bag. It is passed to `bag_delete` whenever the searchBag is to be deleted. The pseudocode for `input_delete` is as follows:

* if the item is not `NULL`, free the memory of the item

There is no error handling for `input_delete`, except for the fact that it will only attempt to free the item if the item is not `NULL`. 

#### void null_delete(void *item)

`null_delete` is a helper function that is passed to `hashtable_delete` in order to deal with the blank strings passed in as items into the `hashtable`. Since there is no memory allocated for these blank strings, they do not need to be freed; however, an item deleting function must still be passed to `hashtable_delete` in order for it to run. Thus, `null_delete` does not do anything but act as a placeholder. `null_delete` takes one parameter: *void * item*, which will be the blank string that needs to be removed from the `hashtable`. There is no code within this function, as it does not serve any purpose except for allowing `hashtable_delete` to run.

### Data structures

#### webpage_t

`webpage_t` is a `struct` object that has three components: *char * url*, *int depth*, and *char * html*. It is defined in `webpage.h`, and more information about its implementation can be found there. The `webpage` module is used to store urls, depth, and html altogether in order to be saved in output files and searched for more urls. It exists in two states: with an uninitialzed html, where the html is `NULL`; and with initialized html, after the `webpage` has been fetched with `webpage_fetch` from the `webpage.h` module. A `webpage` can be made using `webpage_new` and deleted using `webpage_delete`.

#### bag_t

`bag_t` is a `struct` object that has one component: `bagnode *head`, which is also a `struct` that has *void * item* and *struct bagnode *next* components. `bag` is defined in `bag.h`, and more information can be found there. `bag` is implemented as a linked list, with each `bagnode` pointing to the next `bagnode`, and the `head` node pointing to the first `bagnode` in the list. Each `bagnode` holds one *item*; this *item* is the `webpage` that has yet to be searched. `bag` is created by calling `bag_new` and deleted by calling `bag_delete` and passing in an item deleting function, in this case, `input_delete`.

#### hashtable_t

`hashtable_t` is a `struct` object that holds two components: *int num_slots*, and *struct table*, where *struct table* holds a set of *set_t* items. *int num_slots* determines how many slots are in the table; in this case, there are 200 slots in the `hashtable` in `crawler`. The *struct table* is then implemented as a set of sets, holding all of the urls that have already been searched. `hashtable_insert` is used to insert into the table, and since `hashtable` does not allow duplicates, this lets `crawler` know if a url already exists in the table. `hashtable_new` is used to create a `hashtable` and `hashtable_delete` is used to delete a `hashtable`, with `null_delete` being passed to it to delete the blank strings as items, in this case. 

#### set_t

`set_t` is a `struct` object that is utilized by `hashtable`. It holds one component: `struct setnode *head`, which is a `struct` object that holds a `char* key`, `void* item`, and `setnode* next`. `set_t` is implemented as a linked list, with each `setnode` pointing to the next `setnode`. While it isn't explicitly used in `crawler`, `hashtable` uses `set` by having a set of `sets` in its table; this allows for the `hashtable` to hold many `keys` but also not hold any duplicates, since `set` does not allow duplicate keys. 

### Files

* `Makefile` - compilation procedure
* `crawler.c` - the implementation
* `testing.sh` - unit testing file
* `testing.out` - output file for testing
* `DESIGN.md` - design specs
* `REQUIREMENTS.md` - requirements for implementation
* `common.a` - library for common functions in TSE
    * `pagedir.h` - interface for directory functions
    * `pagedir.c` - implementation for directory functions
* `libcs50.a` - library for cs50 modules in TSE
    * `bag.h` - interface of bag
    * `bag.c` - implementation of bag
    * `hashtable.h` - interface of hashtable
    * `jhash.c` - implementation of hashing function
    * `jhash.h` - interface of hashing function
    * `Makefile` - compilation procedure for library
    * `memory.c` - implementation of memory functions
    * `memory.h` - interface for memory functions
    * `set.h` - interface of set
    * `set.c` - implementation of set
    * `webpage.h` - interface for webpage functions
    * `webpage.c` - implementation of webpage functions