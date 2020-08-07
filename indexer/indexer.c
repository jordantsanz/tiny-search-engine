/*
* indexer.c - indexes pages produced by crawler for TSE
*
* input: ./indexer pageDirectory indexFilename
* output: 
*
* Jordan Sanz
* CS50 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "pagedir.h"
#include "file.h"
#include "word.h"
#include "bag.h"
#include "set.h"
#include "counters.h"
#include "hashtable.h"
#include "memory.h"
#include "webpage.h"
#include "index.h"


int indexer(const char* pageDirectory, const char* indexFilename);
bool readDocument(hashtable_t* hash, FILE* fp, int documentID);
char* createFilename(int documentID, const char* pagedirectory);
webpage_t* createPage(FILE* fp);

int main(const int argc, const char* argv[])
{
    // check arguments 

    // check number of arguments
    if (argc != 3) {
        printf("Incorrect number of arguments. \n");
        return 1;
    }

    // check if it is a crawler directory
    if(!isCrawlerDirectory(argv[1])) {
        printf("Directory is not a crawler directory. \n");
        return 2;
    }

    // check if indexFile is openable
    FILE* fp;
    if((fp = fopen(argv[2], "w")) == NULL) {
        printf("Index file cannot be opened for writing. \n");
        return 3;
    }
    fclose(fp);

    // parameters checked successfully; continue

    printf("Parameters checked successfully. Moving to indexing \n");
    int returnValue = indexer(argv[1], argv[2]);
    
    return returnValue;

}


// loops through documents and calls index file maker
int indexer(const char* pageDirectory, const char* indexFilename)
{
    // create first filename template
    printf("Creating filename template. \n");
    int documentID = 1; // counting id



    FILE* fp;  // file pointer

    // create filename
    char* filename = createFilename(documentID, pageDirectory);
    if (filename == NULL) {
        printf("Problem creating filename. \n");
        return 8;
    }

    // initialize the main index
    index_t* index = index_new(850); // large enough number of slots
    if (index == NULL) {
        printf("Index unable to be created. \n");
        return 7;
    }

    // loop of reading documents from pagedirectory
    printf("Reading documents. \n");

    while((fp = fopen(filename, "r")) != NULL) {
        // read document of words and fill hashtable
        if(!readDocument(index, fp, documentID)){
            printf("Unable to read document. \n");
        }
        
        free(filename);

        // create new filename with updated doc id
        documentID += 1;
        filename = createFilename(documentID, pageDirectory);

        // filename was created correctly check
        if (filename == NULL) {
        printf("Problem creating filename. \n");
        return 8;
        }

    }

    free(filename);
    

    printf("Finished reading files. Now saving index. \n");


    // create indexFilename file
    
    if(!index_save(index, indexFilename)){
        printf("Failed to properly save index. \n");
        return 5;
    }

    printf("Saved index. Terminate successfully. \n");


    return 0;
}

/*
* createFilename - creates the file pathname string
*
*/
char* createFilename(int documentID, const char* pageDirectory) 
{
    char* str = (char*) count_malloc(10);  // str of counting id
    char* filename = (char*) count_malloc(strlen(pageDirectory) + 10);  // full filepath

    // memory check
    if (str == NULL || filename == NULL){
        printf("Problem allocating memory. \n");
        return NULL;
    }
    
    strcpy(filename, pageDirectory);
    strcat(filename, "/");
    sprintf(str, "%d", documentID);
    strcat(filename, str);

    free(str);

    return filename;
}


/* readDocument - reads all words in a document
*
*/

bool readDocument(index_t* index, FILE* fp, int documentID)
{
    // make into webpage:
    webpage_t* page = createPage(fp);
    if (page == NULL) {
        return false;
    }

    int pos = 0;
    char* word;

    // read words from the html
    while ((word = webpage_getNextWord(page, &pos)) != NULL) { // error check

        // long enough word
        if (strlen(word) < 3){
            free(word);
            continue;
        }

        // normalize word
        normalizeWord(word);

        // is word already in the hashtable?
        if (index_find(index, word) == NULL){

            // new word in hash
            counters_t* counter = counters_new();
            counters_add(counter, documentID);
            hashtable_insert(index, word, counter);
            free(word);
            continue;
        }

        // word already in hash: no need for new counter
        // do need to check doc id though
        counters_t* counter = index_find(index, word);
        free(word);

        // add one to document ID in counter
        if(counters_add(counter, documentID) == 0) {
            printf("Error incrementing counter for %d \n", documentID);
            return false;
        }

    }
    // free the webpage and return successfully
    webpage_delete(page);
    printf("Words on page indexed correctly. \n");
    fclose(fp);
    return true;
}

/*
* createPage - Creates a new webpage
*/

webpage_t* createPage(FILE* fp)
{

    // first line is url, second is depth, rest is html
    char* url = freadlinep(fp);
    char* depthString = freadlinep(fp);
    char* html = freadfilep(fp);

    if (url == NULL || depthString == NULL || html == NULL) {
        printf("Error creating webpage struct. \n");
        return NULL;
    }

    // get number version of depth
    int depth;
    sscanf(depthString, "%d", &depth);
    free(depthString);

    // create new page
    webpage_t* page = webpage_new(url, depth, html);

    if (page == NULL) {
        printf("Unable to read document. \n");
        return NULL;
    }

    return page;
}