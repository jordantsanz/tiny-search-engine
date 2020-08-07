/*
* index.h - interface for index module
*
* Jordan Sanz
* CS50, 2020
*/


/*************** global structures ********/

typedef hashtable_t index_t; // index data structure

/************* local structures **********/
typedef struct counters counters_t; // outside module stored as item within index


/*************** index_new() ***********/
/*
* Creates a new index structure. 
* Takes input being the number of slots to initialize the index.
* Returns an index_t* index that is blank.
*/
index_t* index_new(const int num_slots);

/******************* index_insert() *************/
/*
* Inserts a counters object into an index.
* Takes an index_t* index, a const char* key, and a counters_t* counter.
* Inserts the counters object into the index with its key being the const char* key.
* Returns a boolean dependent on if the insert was successful.
* If successful, return true; else, return false.
*/
bool index_insert(index_t* index, const char* key, void* item);

/********************* index_delete() **********/
/* Deletes the index structure by calling hashtable delete.
* Will not return anything.
* Takes an index_t* index and a delete function to delete the counters.
*/
void index_delete(index_t* index, void(*itemdelete)(void *item));

/********************* index_find() *****************/
/* 
* Finds the counter stored with any given key in an index.
* Takes a index_t* index and a const char* key
* Returns a counters_t pointer to the counter associated with that key.
*/
counters_t* index_find(index_t* index, const char* key);

/***************** index_iterate() **************/
/*
* Iterates over all of the key/item pairs in the index.
* Takes an index_t* index, a void* arg, and an item function.
*
*/
void index_iterate(index_t* index, void *arg, void (*itemfunc)(void *arg, const char* key, void *item));


/************* index_save() *****************/
/* Saves an index module to a file.
*
* Takes a hashtable_t hashtable pointer and a const char* indexFilename.
* Prints into a file all of the insides of the hashtable
*
* Will print one word per line, with the depth and the counts of that word's instance
* 
* Returns true if successful, false if any error.
*/
bool index_save(hashtable_t* hash, const char* indexFilename);

/************** index_load() ***************/
/* Loads an index module from a file into an index data structure.
*
* Takes a const char* to an index file created by indexer.
* Will create a new index structure and read every line of the file, putting it into the structure.
*
* Will return an index structure of hashtable_t. 
*/
hashtable_t* index_load(const char* oldIndexFilename);

/**************  save_item ************/
/* Saves a counter object to a file
* 
* Takes an arbitrary file pointer, a char* key, and a void* item
* In this case, the key should be a word and the item should be a counter object
* Will iterate over the inner counter values
* 
* Is passed to hashtable_iterate
*/
void save_item (void *arg, const char *key, void *item);

/*************** save_counts ************/
/* Saves the counts from inside of a counter object
* 
* Takes an arbitrary file pointer, an int key, and an int count
* Prints the int key and the int count in "key count" format
* 
* Used as a passed function to counters_iterate
*/
void save_counts (void *arg, const int key, int count);

/************** ct_delete ************/
/* Deletes the counter items from inside of a hashtable
*
* Takes a void* item; here, it will be a counter object
* Will delete the counter object by calling counters_delete on the item
*/
void ct_delete(void *item);