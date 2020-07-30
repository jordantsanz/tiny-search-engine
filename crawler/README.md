# CS50 Lab 4
## Jordan Sanz

### crawler
`crawler` is a part of the Tiny Search Engine (TSE) that fetches webpages at a given URL, takes and writes the HTML on that webpage into an output file, and then searches that webpage for more URLs to repeat the process. 
`crawler` starts with a single URL and then, as long as it doesn't exceed the provided max depth, searches every URL on the seed URL's webpage, continuing until it hits the max depth.

### Usage
`crawler` is a part of the TSE, and implements a `hashtable` of URLs with empty strings as their items, and a `bag` of webpages that need to be visited later on. It utilizes the following functions:

```c
int main(const int argc, const char *argv[]);
int crawler(char* seedURL, char* pageDirectory, const int maxDepth);
webpage_t* initialize_webpage(char* url, int depth, char* html);
bool pagefetcher(webpage_t *page);
bool pagesaver(webpage_t *page, char* pageDirectory, int currentCount);
bool pagescanner(webpage_t *page, int currentDepth, bag_t *searchBag, hashtable_t *urlHash)
void input_delete(void *item);
void null_delete(void *item);
```

### Implementation

A full implementation spec is found in the IMPLEMENTATION.md file. 

### Assumptions

The `seedURL` entered as input must be one that is internal, as to not overload the TSE. The `pageDirectory` given as input must be an existing directory, and the `maxDepth` must be a number greater or equal to 0 and less than or equal to 10. 

The number of slots in the hashtable is initialized to 200. Additionally, the id counter to create the output files has enough memory for ten digits; this will not be a problem, since the hashtable has a more limiting circumstance, but it is an assumption nonetheless. 

### Files

A full list of files used is found in the IMPLEMENTATION.md spec.

### Exit Codes

* 1: wrong number of parameters
* 2: url is not internal
* 3: directory does not exist
* 4: maxDepth is out of bounds
* 5: seedWebpage memory unable to be allocated
* 6: bag memory unable to be allocated
* 7: hashtable memory unable to be allocated
* 8: page html could not be extracted
* 9: page could not be saved

### Compilation

To compile, simply `make crawler.o`. 

### Testing

The `testing.sh` program tests various forms of incorrect command line arguments ot ensure command-line parsing and validation of those parameters works correctly. It also crawls simple sets of pages, at various depths and with various seed pages.

`testing.sh` also explores the Wikipedia playground at smaller depths, and it tests playgrounds at greater depths. The output is saved in testing.out. 
