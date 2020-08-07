/*
* word.h - header file for word.c program
*
* 
* Jordan Sanz
* CS50, 2020
*/

/************************ normalizeWord()*************/
/* Return a word in all lowercase alphabet.
* 
* Caller provides:
*   valid pointer to a string that is all uppercase, lowercase, or a mixture of both.
* We return:
*   pointer to a string that is all lowercase letters.
*/

char* normalizeWord(char* word);