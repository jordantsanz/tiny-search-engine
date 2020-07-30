/*
* pagedir.h - a header file for pagedir.c
*
* pagedir is a module that assists the Tiny Search Engine,
* allowing crawler, index, and query to check to make sure a
* directory for page writing is available and openable.
*
*
*/
#include <stdio.h>
#include <stdbool.h>
#include "../libcs50/webpage.h"

/************* check_directory ***********/
/*
* Returns a boolean dependent on if a passed directory exists or not
* If the directory exists and is openable, return true
* If the directory does not exist or is not openable, return false
* check_directory takes a pointer to a directory
*/
bool check_directory(const char* directory);

/************** pagesaver ************/
/*
* Saves an output text file for the webpage passed into it. Will print the url on the first line of the file,
* the max depth on the second, and the HTML of the page for the rest of the file. 
* Returns true if the process finishes successfully, and returns false otherwise.
* Takes a webpage_t page pointer, a pageDirectory string, and the current id count for naming the file.
* Each file is named with a unique number corresponding to the order the URLs are searched.
*/
bool pagesaver(webpage_t *page, const char* pageDirectory, int currentCount);