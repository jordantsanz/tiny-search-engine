/*
* querier.c - querier program for Tiny Search Engine
* 
* Usage: ./querier pageDirectory indexFilename
*
* Input: stream of words, separated by "and" or "or" or not separated at all
*
* Output: printed lines of documents satisfying query ranked in order of highest score
*
*
* Jordan Sanz
* CS50, Summer 2020
*/


/*
* Exit codes:
* 1: wrong amount of arguments
* 2: pageDirectory not a crawler directory
* 3: indexFile not readable
* 4: 
* 5:
* 6: incorrect word in query
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "pagedir.h"
#include "word.h"
#include "file.h"
#include "bag.h"
#include "set.h"
#include "hashtable.h"
#include "counters.h"
#include "memory.h"
#include "webpage.h"
#include "index.h"


/********** LOCAL DATA TYPES ****************************************************************/

/****************** twocts **********************
* Struct that holds two counters
*************************************************/
struct twocts {
    counters_t *result;
    counters_t *another;
};

/********************* documentResult **************
* Struct thaat holds a docID, a count of words, and a URL
*************************************************/
typedef struct documentResult {
    int docID;
    int count;
} documentResult_t;

/****************************************************************************************/


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

/****************************************************************** FUNCTIONS ********************************/
/********************************** main *********************
* Parses command-line arguments to validate them
* Returns on EOF reached
***************************************************************/
int main(const int argc, const char* argv[])
{
    // checks number of arguments
    if (argc != 3) {
        printf("Please only provide two arguments after the program name. \n");
        return 1;
    }

    if(!isCrawlerDirectory(argv[1])) {
        printf("Page directory must be a crawler directory. \n");
        return 2;
    }

    FILE* fp;
    if((fp = fopen(argv[2], "r")) == NULL) {
        printf("Index file is not readable. \n");
        return 3;
    }

    printf("Parameters validated. Starting querier. \n");
    int exitcode = querier(argv[1], argv[2]);

    fclose(fp);

    return exitcode;
}

/*********************************** querier ****************
* Runs querier loop prompting for new queries
* Calls parse query to parse new queries
***************************************************************/
int querier(const char* pageDirectory, const char* indexFilename)
{
    // initialize index based on number of lines in file and load
    //index_t* index = index_load(indexFilename);

    printf("Loaded. Please enter your search query: \n");

    char* str;
    printf("Query? \n");

    // cycle through queries
    while ((str = readlinep()) != NULL) {
        parseQuery(str, indexFilename, pageDirectory);
        free(str);

    printf("Query? \n");

    }
    
    return 0;

}

/************************************* parseQuery **************************
* Parses the query given by the user
* Calls all of the main query functions
***************************************************************************/
int parseQuery(char* str, const char* indexFilename, const char* pageDirectory) {

    // counter
    counters_t* orseq = counters_new();

    // index loaded-in
    index_t* indexModule = index_load(indexFilename);

    if(indexModule == NULL){
        printf("Cannot load index from filename. \n");
        return 9;
    }

    // list of words in query
    char** words;
    words = assertp(count_malloc(sizeof(char*) * (strlen(str) / 2)), "Cannot allocate memory for words list. \n");

    // separate query into pieces
    int indicesInList = separateQuery(str, words);

    // no words entered in query
    if (words[0] == NULL) {
        printf("No words found. \n");
        free(words);
        index_delete(indexModule, ct_delete);
        counters_delete(orseq);
        return 4;
    }

    // print the pretty query:
    if (!printCleanedQuery(words, indicesInList)) {
        free(words);
        index_delete(indexModule, ct_delete);
        counters_delete(orseq);
        return 6;
    }

    // calculate the scoring of the query
    if(!calculateQuery(words, indicesInList, indexModule, orseq)){
        free(words);
        index_delete(indexModule, ct_delete);
        counters_delete(orseq);
        return 7;
    }

    orderResults(orseq, pageDirectory);

    counters_delete(orseq);
    index_delete(indexModule, ct_delete);
    free(words);

    return 0;
}
/********************** separateQuery ********************
* Separates a query string into words
* Reads through entire query and keeps two indices
* Puts one index at front of word and one index at end of word
* Puts null pointer at end of word and reads entire word
* Returns number of words in list of words found
*********************************************************/
int separateQuery(char* str, char** words)
{
    // front and back indices
    int index1 = 0;
    int index2;

    // cover any beginning spaces
    while (isspace(str[index1])){
        index1++;
    }
    
    index2 = index1; // like setting them both to 0 basically

    int endval = strlen(str) - 1;

    while(isspace(str[endval]) || str[endval] == '\0'){
        endval--; // moves it to first letter
    }
    endval++; // now on very last place for string


    int wordPositionInList = 0;

    // while index2 isn't at end of query
    while(index2 != endval){

        // if index2 reaces end of query somehow, get out!
        if(str[index2] == '\0'){
            break;
        }

        // if index2 is not on a space, move it forward and continue to next character
        if(!isspace(str[index2])) {
            printf("%c \n", str[index2]);
            index2 += 1;
            continue;
        }

        // if index 1 and 2 are on same space, move index 2 forward one space
        if (index1 == index2) {
            index2 += 1; // move index 2 forward one
            continue;
        }

        // while index1 isn't on a letter, increment forward
        if(!isalpha(str[index1])) {
            index1 += 1;
            continue;
        }

        // if index1 jumped over index2, then move index2 forward
        if (index1 >= index2) {
            index2 += 1;
            continue;
        }

        // if reaching here, should now have:
        // 1. index 1 < index 2
        // 2. letter1 is a char
        // 3. letter2 is the first space after a set of chars
        // 4. index2 isn't on a \0
        // now can try and read the word
        if(index1 < index2 && isalpha(str[index1]) && isspace(str[index2])) {
            str[index2] = '\0';

            words[wordPositionInList] = &str[index1];

            wordPositionInList += 1;

            while (index1 != index2) {
                index1 += 1;
            }

            if (index2 != strlen(str) - 1){ // if not at end of query
                index2 += 1; // move index2 off of null
                index1 += 1; // move index1 off of null
            }
        }
    }

    // if there's still letters left, grab last word
    if (isalpha(str[index1])) {
        words[wordPositionInList] = &str[index1];
    }

    return wordPositionInList;
}


/*************** printCleanedQuery *******************
Cleans the query and prints it out
* Validates and checks the input words as well
* Will print the query out after validating
* Will return for new query if bad input, with error message printed
*/
bool printCleanedQuery(char** words, int indicesInList)
{
    char* and = "and";
    char* or = "or";

    for (int index = 0; index <= indicesInList; index++) {

        // normalize word and set as word at that place in list
        char* word = normalizeWord(words[index]);
        if (word == NULL) {
            printf("Please put only letters in each word. No special characters or numbers. \n");
            return false;
        }
        words[index] = word;

        // check that first/last word isn't and/or
        if (index == 0 || index == indicesInList) {

            // if and
            if (strcmp(and, word) == 0){
                printf("%s \n", word);
                printf("Word cannot be and at start or end of list. \n");
                return false;
            } 

            // if or
            if (strcmp(or, word) == 0) {
                printf("%s \n", word);
                printf("First and last words in query cannot be 'or.'");
                return false;
            }
        }
    }

    // print query words
    printf("Query found: ");
    for (int index=0; index <= indicesInList; index++) {
        printf("%s ", words[index]);
    }

    // print new line
    printf("\n");

    return true;
}


/************************ calculateQuery *******************
* Will calculate the scores for all of the documents 
* Loads the index module and runs the and/or precendence algorithm
* Reads through all of the words, returns true if successful, false if not
**************************************************************************/
bool calculateQuery(char** words, int indicesInList, index_t* indexModule, counters_t* orseq)
{
    char* conjunction = NULL;
    bool madeBlankAnd = false;

    counters_t* andseq = index_find(indexModule, words[0]);

    // have to make null counter for andseq if nothing found
    if (andseq == NULL) {
        andseq = counters_new();
        madeBlankAnd = true;
    }

    counters_t* counters_2;

    // loop through all words 
    for (int index = 1; index <= indicesInList; index++) {

        // is next word a conjunction and?
        if (strcmp("and", words[index]) == 0) {

            // already have a conjunction
            if (conjunction != NULL) {
                printf("Bad query. Please only put an 'and' in-between two other words that AREN'T 'and' or 'or'. \n");

                // free before exiting if necessary
                if(madeBlankAnd){
                    free(andseq);
                }

                // return with error
                return false;
            }

            // else, conjunction is and
            conjunction = "and";
            continue;
        }

        // is next word a conjunction or?
        if (strcmp("or", words[index]) == 0) {

            // already have a conjunction
            if (conjunction != NULL) {
                printf("Bad query. Please only put an 'or' in-between two other words that AREN'T 'and' or 'or'. \n");

                // free before exiting if necessary
                if(madeBlankAnd){
                    free(andseq);
                }

                // return with error
                return false;
            }

            // else, conjunction is or
            conjunction = "or";
        }

        // if andseq is null, get word and go to next word
        if (andseq == NULL) {

            // find new andseq
            andseq = index_find(indexModule, words[index]);
            if (andseq == NULL){
                andseq = counters_new();
                madeBlankAnd = true;
            }

            // reset or, because now can be an and
            conjunction = NULL;
            continue;
        }

        // have and query
        if (conjunction == NULL || strcmp("and", conjunction) == 0) {
            counters_2 = index_find(indexModule, words[index]);

            // if cannot find a word for counters_2, make blank counters, free as well
            if (counters_2 == NULL){
                counters_2 = counters_new();
                andQuery(andseq, counters_2);
                counters_delete(counters_2);
                conjunction = NULL;
                counters_2 = NULL;
                continue;
            }
            
            // perform an and query
            andQuery(andseq, counters_2);

            // free and reset
            counters_2 = NULL;
            conjunction = NULL;
            continue;
        } 

        // have or query
        if (strcmp("or", conjunction) == 0) {

            // perform an or query
            orQuery(andseq, orseq);

            // if made one outside of indexModule, free
            if (madeBlankAnd){
                free(andseq);
            }

            // reset
            andseq = NULL;
            continue;
        }
    }

    // after for loop, merge stuff in 'and' sequence with 'or' sequence
    if (orseq == NULL){
        printf("No documents found. \n");
        return false;
    }

    if(andseq == NULL){
        return true;
    }

    orQuery(andseq, orseq);

    return true;
}

/******************* orQuery ********************
* Finds the sum of two counters passed in
* Performs a union
*************************************************/
void orQuery(counters_t* andseq, counters_t* orseq)
{
    assertp(andseq, "andseq is invalid. \n");
    assertp(orseq, "orseq is invalid. \n");

    struct twocts args = {orseq, andseq};
    counters_iterate(andseq, &args, union_helper);
}


/******************* andQuery ********************
* Finds the min of two counters passed in
* Performs an intersection
*************************************************/
void andQuery(counters_t* andseq, counters_t* counters_2)
{
    assertp(andseq, "andseq is invalid. \n");
    assertp(counters_2, "counter is invalid.\n");

    struct twocts args = {andseq, counters_2};
    counters_iterate(andseq, &args, intersect_helper);
}

/*********************** intersect_helper ***********
* Simply finds the minimum of two counts in two counters
* sets the main counter to that minimum value
****************************************************/
void intersect_helper(void *arg, const int key, const int count)
{
    struct twocts *two = arg;

    counters_set(two->result, key, min(count, counters_get(two->another, key)));
}

/************************* union_helper ***************
* Finds the sum of the counts of two counters
* Sets the count of the main counter to that value
******************************************************/
void union_helper(void *arg, const int key, const int count)
{
    struct twocts *two = arg;

    int sum = count + counters_get(two->result, key);

    printf("sum: %d \n", sum);

    counters_set(two->result, key, sum);
}


/*************************** min *************************
* Returns the minimum value from two integers
*********************************************************/
int min(const int a, const int b) 
{
    return (a < b ? a : b);
}

/**************************** orderResults ****************
* Orders the documentIDs in a counter by count
* Iterates through all of the documentIDs and sorts by count in list
* Returns true if successful, false otherwise
**********************************************************/
bool orderResults(counters_t* orseq, const char* pageDirectory) 
{
    // find how many documents in final counter
    int documentCount = 0;
    counters_iterate(orseq, &documentCount, countDocumentHelper);
    counters_print(orseq, stdout);

    // check for no documents
    if (documentCount == 0){
        printf("No documents found. \n");
        return true;
    }

    // create list of results
    documentResult_t **results;
    results = assertp(count_malloc(sizeof(documentResult_t) * sizeof(documentCount)), "cannot allocate memory for results. \n");

    for(int i = 0; i < documentCount; i++){
        results[i] = NULL;
    }

    counters_iterate(orseq, results, createDocumentHelper);

    // sort list of documents with insertion sort
    insertionSort(results, documentCount);

    // prints sorted array of documents and finds url
    printResults(results, pageDirectory, documentCount);

    for (int i=0; i < documentCount; i++){
        free(results[i]);
    }
    free(results);

    return true;
    

}

/***************************** countDocumentHelper ***************
* Used by orderDocumentResults
* Counts the number of documents in the counter
*****************************************************************/
void countDocumentHelper(void *arg, const int key, const int count)
{
        int *nitems = arg;

        if (nitems != NULL)
            (*nitems)++;
}

/********************** createDocumentHelper *******************
* Creates document and adds to the list of documents
* Helper for counters_iterate
*****************************************************************/
void createDocumentHelper(void *arg, const int key, const int count)
{
    documentResult_t** results = arg;

    // create document
    documentResult_t* document;
    document = malloc(sizeof(documentResult_t*));
    document->docID = key;
    document->count = count;
    int i = 0;

    while (results[i] != NULL){
        i++;
        }
    results[i] = document; // insert document into list

}

/***************************** insertionSort *************
* sorts array of documentResult_t struct items
**********************************************************/
void insertionSort(void* arg, int n)
{
    documentResult_t** results = arg;
    // insertion sort algorithm adapted from geeksforgeeks.com
    int i, j;
    documentResult_t* key;

    for (i = 1; i < n; i++){
        key = results[i];
        j = i - 1;

        while (j >= 0 && results[j]->count < key->count){
            results[j + 1] = results[j];
            j = j - 1;
        }

        results[j + 1] = key;
    }
}
/********************************** printResults ***********
* Cycles through a list of document results
* Prints all of them by score, count, and url
**********************************************************/
bool printResults(void* arg, const char* pageDirectory, int documentCount)
{
    documentResult_t** results = arg;

    int bad = 0;
   for (int index = 0; index < documentCount; index++){
       if (results[index]->count == 0){
           bad++;
       }
   }
 
    documentCount = documentCount - bad;

    if(documentCount == 0){
        printf("No documents found. \n");
        return false;
    }

    printf("Matched %d results (ranked): \n", documentCount);

    for (int index = 0; index < documentCount; index++){
        if(results[index]->count != 0){
        char* url = findURL(results[index]->docID, pageDirectory);
        printf("Score:    %d DocID:   %d   URL: %s \n ", results[index]->count, results[index]->docID, url);
        free(url);
        }
    }

    return true;
}

/*********************************** findURL ****************
* Finds the URL of a documentID by parsing the document
* Reads the first line of the document and returns it
*************************************************************/
char* findURL(const int docID, const char* pageDirectory)
{
    // create file pointer
    FILE *fp;
    char * str = assertp((char*) count_malloc(sizeof(docID)), "cannot make docid into string. \n");
    char* filename = assertp((char*) count_malloc(strlen(pageDirectory) + sizeof(str)), "cannot make filename. \n");

    // put pageDirectory on base of filename string
    strcpy(filename, pageDirectory);
    strcat(filename, "/");
    sprintf(str, "%d", docID);

    // put docID in string version on filename string
    strcat(filename, str);
    fp = fopen(filename, "r");

    // cannot open file
    if (fp == NULL){
        printf("Problem getting URL. \n");
        return NULL;
    }

    // read file and got URL
    char* url = freadlinep(fp);

    // free memory
    free(filename);
    free(str);
    fclose(fp);

    return url;
}

