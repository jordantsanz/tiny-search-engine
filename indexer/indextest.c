/*
* indextest.c - tests an index page by making a copy
*
* input: ./indextest oldIndexFilename newIndexFilename
* output: prints the oldIndexFilename contents into the newIndexFilename file.
*
* Jordan Sanz
* CS50 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../libcs50/hashtable.h"
#include "../libcs50/webpage.h"
#include "../libcs50/bag.h"
#include "../libcs50/memory.h"
#include "../common/index.h"
#include "../libcs50/file.h"


int indextest(const char* oldIndexFilename, const char* indexFilename);

// main for indextest
int main(const int argc, const char* argv[])
{
    // number of arguments
    if (argc != 3) {
        printf("Wrong number of arguments. Terminate \n");
        return 1;
    }

    // unable to open index
    FILE* fp;
    if ((fp = fopen(argv[1], "r")) == NULL) {
        printf("Old index file unable to be opened. \n");
        return 2;
    }
    fclose(fp);

    // cannot open new indexfile
    FILE* fpnew;
    if ((fpnew = fopen(argv[2], "w")) == NULL) {
        printf("Unable to open new indexfile for writing. \n");
        return 3;
    }

    // close indexfile
    fclose(fpnew);
    printf("Arguments checked successfully. Moving to index copying. \n");

    // run indextest
    int returncode = indextest(argv[1], argv[2]);

    return returncode;
}

// runs indexload and indexsave
int indextest(const char* oldIndexFilename, const char* indexFilename)
{
    // loads in indexfile information
    printf("Going to load. \n");

    index_t* index = index_load(oldIndexFilename);

    // memory check for index
    if (index == NULL) {
        printf("Unable to load index correctly. \n");
        return 4;
    }


    // save indexfile
    if(!index_save(index, indexFilename)) {
        printf("Unable to save index correctly. \n");
        return 5;
    }

    // delete hashtable and return correctly
    printf("Index saved. \n");

    return 0;

}