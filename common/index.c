/* 
* index.c - index module for utilization in indexer
*
* 
* Jordan Sanz
* CS50, 2020
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../libcs50/file.h"
#include "../libcs50/counters.h"
#include "../libcs50/hashtable.h"
#include "../libcs50/memory.h"


bool index_save(hashtable_t* hash, const char* indexFilename);
void save_item(void *arg, const char* key, void *item);
void save_counts(void* arg, const int key, int count);
void ct_delete(void* item);
hashtable_t* index_load(const char* oldIndexFilename);

/******************** global types ***************/
typedef hashtable_t index_t;


/******************** index_new() **************/
index_t* index_new(const int num_slots)
{
    return (index_t*) hashtable_new(num_slots);
}

/***************** index_insert() ***************/
bool index_insert(index_t* index, const char* key, void* item)
{
    if(hashtable_insert(index, key, item)){
        return true;
    }
    return false;
}

/******************** index_delete() *************/
void index_delete(index_t* index, void(*itemdelete)(void *item))
{
    hashtable_delete(index, itemdelete);
}

/********************** index_find() *************/
counters_t* index_find(index_t* index, const char* key)
{
    return (counters_t*) hashtable_find(index, key);
}

/******************** index_iterate() ***********/
void index_iterate(index_t* index, void *arg, void (*itemfunc)(void *arg, const char *key, void *item))
{
    hashtable_iterate(index, arg, itemfunc);
}

/************** index_save() ************/
/*
* Saves an index's contents to an output file
*/
bool index_save(index_t* index, const char* indexFilename)
{
    // check to make sure hashtable exists
    if (index == NULL) {
        printf("Index module does not exist. \n");
        return false;
    }

    // open file and iterate through hashtable to print
    FILE* fp = fopen(indexFilename, "w");
    index_iterate(index, fp, save_item);

    // cleanup
    index_delete(index, ct_delete);
    fclose(fp);
    return true;
}

/******** helper to save item in hashtable ********/
void save_item (void *arg, const char *key, void *item)
{
	FILE *fp = arg; 
	counters_t *c = item;

	fprintf(fp, "%s", key);
	counters_iterate(c, fp, save_counts);
	fprintf(fp, "\n");
}

/******** helper to save items in counters ********/
void save_counts (void *arg, const int key, int count)
{
	FILE *fp = arg; 
	fprintf(fp, " %d %d", key, count);
}

/*********** deletes counter items ************/
void ct_delete(void *item)
{
	counters_t* ct = item;

	if (ct != NULL) {
		counters_delete(ct);
	}
}
/*************** index_load() ***************/
/*
* Loads in index from a saved index file
*/
index_t* index_load(const char* oldIndexFilename)
{
    // find lines in file to make hash
    FILE* fpold = fopen(oldIndexFilename, "r");
    int num_slots = lines_in_file(fpold);
    index_t* index = index_new(num_slots);

    if (index == NULL) {
        printf("Hashtable memory unable to be allocated. \n");
        return NULL;
    }

    // word, id, count to read from
    char* word;
    int id;
    int count;

    // while there is a word in the file still
    while ((word = freadwordp(fpold)) != NULL) {

            // create a new counter
            counters_t* counter = counters_new();

            if (counter == NULL) {
                printf("Memory for counter unable to be allocated. \n");
                return NULL;
            }
        
            // while there are numbers in a row
            while (fscanf(fpold, "%d %d", &id, &count) == 2){

                // set them in the counter
                counters_set(counter, id, count);
            }

            // add to index, free word, and repeat
            index_insert(index, word, counter);
            free(word);
    }
    
    // close the file and return successfully
    fclose(fpold);
    printf("Index loaded successfully. \n");
    return index;
}
