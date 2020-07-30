/*
* crawler.c - program to crawl webpages for URLs in TSE
*
* usage - $ crawler seedURL pageDirectory maxDepth
*
* input - command-line parameters only
* output - unique files for each explored webpage
*
* Jordan Sanz
* CS50
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "pagedir.h"
#include "bag.h"
#include "set.h"
#include "hashtable.h"
#include "memory.h"
#include "webpage.h"

bool pagefetcher(webpage_t *page);
bool pagescanner(webpage_t *page, int currentDepth, bag_t *searchBag, hashtable_t *urlHash);
webpage_t* initialize_webpage(char* url, int depth, char* html);
int crawler(char* seedURL, const char* pageDirectory, const int maxDepth);
void input_delete(void* item);
void null_delete(void* item);


int main(const int argc, const char *argv[])
{
    // verify correct number of parameters
    if (argc != 4){
        printf("Wrong amount of parameters provided. \n");
        return 1;
    }

    // initialize argument variables
    char* url = assertp(count_malloc(sizeof(char)*(strlen(argv[1]) + 1)), "url copy");
    strcpy(url, argv[1]);

    // check seedURL is internalized
    if(!IsInternalURL(url)){
        printf("URL is not internal. \n");
        return 2;
    }

    // check that directory exists
    if(!check_directory(argv[2])){
        return 3;
    }

    int maxDepth;
    if(sscanf(argv[3], "%d", &maxDepth) != 1){
        printf("Max Depth is not a number.");
        return 4;
    }

    if(maxDepth > 10 || maxDepth < 0){
        printf("Max Depth is out of bounds. Please use a number between 0 and 10. \n");
        return 4;
    }


    // call to crawler
    printf("Parameters verifed: crawling webpages \n");
    int exitCode = crawler(url, argv[2], maxDepth);

    // return with crawler's exitcode
    return exitCode;
}

int crawler(char* seedURL, const char* pageDirectory, const int maxDepth)
{

    // initialize webpage
    webpage_t *page = initialize_webpage(seedURL, 0, NULL);
    if (page == NULL){
        return 5;
    }

    // initialize bag for searching 
    bag_t *searchBag = bag_new();
    if (searchBag == NULL){
        printf("Bag memory unable to be allocated. \n");
        return 6;
    }

    // insert seed webpage into searchBag
    bag_insert(searchBag, page);

    // initialize hashtable for urls searched
    hashtable_t *urlHash = hashtable_new(200);

    // check to make sure allocated correctly
    if (urlHash == NULL){
        printf("Hashtable memory unable to be allocated. \n");
        return 7;
    }

    // insert seedURL into urlHash
    hashtable_insert(urlHash, webpage_getURL(page), " ");


    // keeps track of file name id
    int currentCount;
    currentCount = 0;

    // loop to crawl through webpages
    while((page = bag_extract(searchBag)) != NULL){
        
        currentCount += 1;
        printf("Beginning to crawl webpage %d \n", currentCount);
        // fetch page; if error, return
        if(!pagefetcher(page)){
            webpage_delete(page);
            continue;
        }

        
        // write page; if error, return
        if(!pagesaver(page, pageDirectory, currentCount)){
            return 9;
        }

        // if not final depth, find new pages
        if(webpage_getDepth(page) < maxDepth){
            pagescanner(page, webpage_getDepth(page), searchBag, urlHash);
        }
        printf("Finished with webpage %d \n", currentCount);
        webpage_delete(page);
    }

    // return successfully
    bag_delete(searchBag, input_delete);
    hashtable_delete(urlHash, null_delete);

    return 0;
}

webpage_t *initialize_webpage(char* url, int depth, char* html)
{
    // make new webpage
    webpage_t *webpage = webpage_new(url, depth, html);

    // if webpage is null, error; return
    if (webpage == NULL){
        printf("Webpage memory unable to be allocated. \n");
        return NULL;
    }
    return webpage;
}


bool pagefetcher(webpage_t *page){

    // adds html to page
    bool htmlWorked = webpage_fetch(page);
    // if didn't work, error
    if(!htmlWorked){
        printf("Page html for url %s could not be extracted. \n", webpage_getURL(page));
        return false;
    }


    // successful exit from function
    return true;
}

bool pagescanner(webpage_t *page, int currentDepth, bag_t *searchBag, hashtable_t *urlHash)
{
    // go through entire webpage and look for URLs
    int pos = 0;
    char *result;

    // while there are URLS
    while((result = webpage_getNextURL(page, &pos)) != NULL){
        
        // normalize URL
        NormalizeURL(result);
        // if not internal, then continue to next URL
        if(!IsInternalURL(result)){
            free(result);
            continue;
        }

        // if can't insert into hash, duplicate; continue to next URL
        if(!hashtable_insert(urlHash, result, " ")){
            free(result);
            continue;
        }

        // make new webpage for this URL
        webpage_t *newPage = initialize_webpage(result, currentDepth + 1, NULL);

        // add webpage to bag to search
        bag_insert(searchBag, newPage);

    }

    return true;
}


// deletes items for bag
void input_delete(void *item)
{
  if (item != NULL) {
    free(item);   
  }
}

void null_delete(void *item)
{
    // doesn't do anything, since the item to be freed is a blank string
}

