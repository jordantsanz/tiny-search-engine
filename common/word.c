/*
* word.c - program for normalizing words
*
* Jordan Sanz
* CS50, 2020
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


// normalizes word 
char* normalizeWord(char* word)
{
    for (int i = 0; i < strlen(word); i++) {
        if(!isalpha(word[i])){
            return NULL;
        }
        word[i] = tolower(word[i]);
    }

    return word;
}