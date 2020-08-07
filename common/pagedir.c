/*
* pagedir.c - a module used by TSE to work with directories
*
*
* Jordan Sanz
* CS50
*
*
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../libcs50/webpage.h"
#include "../libcs50/memory.h"

bool check_directory(const char* directory)
{
    char* pathName = count_malloc(strlen(directory) + 10);
    strcpy(pathName, directory); 
    strcat(pathName, "/.crawler");

    FILE* fp;
    // check that directory exists
    if((fp = fopen(pathName, "w")) == NULL){
        printf("Directory does not exist.\n");

        // does not exist
        return false;
    }

    // exists
    fclose(fp);
    free(pathName);
    
    return true;
}

bool pagesaver(webpage_t *page, const char* pageDirectory, int currentCount)
{
    // write new file
    FILE *fp;
    char * str = (char*) count_malloc(sizeof(char) * 10);
    char* filename = (char*) count_malloc(strlen(pageDirectory) + sizeof(str));
    strcpy(filename, pageDirectory);
    strcat(filename, "/");
    sprintf(str, "%d", currentCount);
    strcat(filename, str);
    fp = fopen(filename, "w");

    // problem writing new file
    assertp(fp, "cannot open file for writing \n");

    // normalize URL and write first two lines
    NormalizeURL(webpage_getURL(page));
    fprintf(fp, "%s \n", webpage_getURL(page)); // writes url
    fprintf(fp, "%d \n", webpage_getDepth(page)); // write depth
    fprintf(fp, "%s \n", webpage_getHTML(page));
    fclose(fp);

    count_free(filename);
    count_free(str);
    return true;
}

bool isCrawlerDirectory(const char* pageDirectory)
{
    char* filename = count_malloc(strlen(pageDirectory) + 10);
    strcpy(filename, pageDirectory); 
    strcat(filename, "/.crawler");

    FILE* fp;
    if ((fp = fopen(filename, "w")) == NULL) {
        printf("Not a crawler directory. Error. \n");
        return false;
    }
    fclose(fp);
    free(filename);

    return true;
}