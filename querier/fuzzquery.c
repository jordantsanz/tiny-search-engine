/* 
 * fuzzquery - generate a series of random queries for testing querier
 * 
 * usage: 
 *   fuzzquery indexFile numQueries randomSeed
 *
 * David Kotz - May 2016, 2017, 2019
 * updated by Xia Zhou, 2016, 2020
 */

#include <stdio.h>
#include <stdlib.h> // random, srandom
#include <string.h> // strchr
#include "memory.h"
#include "file.h"

/* The random() and srandom() functions are provided by stdlib,
 * but for some reason not declared by stdlib.h, so declare here.
 */
long int random(void);
void srandom(unsigned int seed);

/**************** file-local global variables ****************/
static char *program;
static const char *UnixDictionary = "/usr/share/dict/words";
// If that file is not available, install it with:
//     sudo apt-get install wamerican

/**************** file-local global types ****************/
typedef struct {
  int nwords;                 // number of words in the list
  char **words;               // array of words
} wordlist_t;

/**************** local functions ****************/
static void parse_args(const int argc, char *argv[], 
                       char **indexFilename, int *numQueries, int *randomSeed);
static wordlist_t *wordlist_load(const char *indexFilename);
static void wordlist_delete(wordlist_t *words);
static void generate_query(const wordlist_t *wordlist, 
                           const wordlist_t *dictionary);

/**************** main ****************/
int
main(const int argc, char *argv[])
{
  char *indexFilename;
  int numQueries;
  int randomSeed;

  // parse the arguments
  // function exits if any errors in parsing
  parse_args(argc, argv, &indexFilename, &numQueries, &randomSeed);

  // seed random number generator
  srandom(randomSeed);

  // load an array full of words from the given index file
  wordlist_t *wordlist = wordlist_load(indexFilename);
  if (wordlist == NULL) {
    fprintf(stderr, "%s cannot load words from dictionary '%s'\n",
            program, indexFilename);
    exit(9);
  }
  
  // load an array full of words from the Unix dictionary
  wordlist_t *dictionary = wordlist_load(UnixDictionary);
  if (dictionary == NULL) {
    fprintf(stderr, "%s cannot load words from dictionary '%s'\n",
            program, UnixDictionary);
    exit(9);
  }
  
  fprintf(stderr, "%s: generating %d queries from %d words\n", 
          program, numQueries, wordlist->nwords);

  // generate random queries, using that wordlist
  for (int q = 0; q < numQueries; q++)
    generate_query(wordlist, dictionary);

  wordlist_delete(wordlist);
  wordlist_delete(dictionary);

  // count_report(stderr, "end of main");
}

/**************** parse_args ****************/
/* Parse the command-line arguments, filling in the parameters;
 * if any error, print to stderr and exit.
 */
static void
parse_args(const int argc, char *argv[], 
           char **indexFilename, int *numQueries, int *randomSeed)
{
  char extra;

  /**** usage ****/
  program = argv[0];
  if (argc != 4) {
    fprintf(stderr, "usage: %s indexFile numQueries randomSeed\n", program);
    exit(1);
  }

  /**** indexFile ****/
  *indexFilename = argv[1];

  /**** numQueries ****/
  if (sscanf(argv[2], "%d%c", numQueries, &extra) != 1 || *numQueries < 0) {
    fprintf(stderr, "usage: %s: invalid numQueries '%s'\n", program, argv[2]);
    exit (2);
  }
  
  /**** randomSeed ****/
  if (sscanf(argv[3], "%d%c", randomSeed, &extra) != 1 || *randomSeed < 0) {
    fprintf(stderr, "usage: %s: invalid randomSeed '%s'\n", program, argv[3]);
    exit (3);
  }
}

/**************** wordlist_load ****************/
/* load the set of words in the given index file,
 * saving them in a freshly-allocated array;
 * the caller must later call words_delete() on the result.
 */
static wordlist_t *
wordlist_load(const char *indexFilename)
{
  if (indexFilename == NULL) {
    return NULL;
  }

  FILE *fp = fopen(indexFilename, "r");
  if (fp == NULL) {
    fprintf(stderr, "%s: cannot open index file '%s'\n", 
            program, indexFilename);
    return NULL;
  }

  // How many words will we have to read?
  int nwords = lines_in_file(fp);

  if (nwords == 0) {
    fprintf(stderr, "%s: index file '%s' has no words\n",
            program, indexFilename);
    fclose(fp);
    return NULL;
  }

  // Make array to hold the words
  char **words = count_calloc_assert(nwords, sizeof(char*), "words[]");

  // wrap the result in a wordlist
  wordlist_t *wordlist = count_malloc_assert(sizeof(wordlist_t), "wordlist");
  wordlist->nwords = nwords;
  wordlist->words = words;

  // read in all the words  
  for (int w = 0; w < nwords; w++) {
    char *word = freadlinep(fp);
    if (word == NULL) {
      // this should not happen - but clean up if it does.
      fprintf(stderr, "%s: index file '%s' ends unexpectedly\n",
              program, indexFilename);
      wordlist->nwords = w;
      wordlist_delete(wordlist);
      fclose(fp);
      return NULL;
    } else {
      // truncate and save the word
      char *space = strchr(word, ' ');
      if (space != NULL)
        *space = '\0';
      words[w] = word;
    }
  }
  fclose(fp);

  return wordlist;
}

/**************** wordlist_delete ****************/
/* free the memory allocated by wordlist_load
 */
static void
wordlist_delete(wordlist_t *wordlist)
{
  assertp(wordlist, "wordlist_delete words");

  char **words = wordlist->words;
  int nwords = wordlist->nwords;
  for (int w = 0; w < nwords; w++) {
    free(words[w]);           // was allocated by readlinep()
  }

  count_free(wordlist->words);
  count_free(wordlist);
}

/**************** generate_query ****************/
/* generate one random query and print to stdout.
 * pull random words from the wordlist and from the dictionary.
 */
static void
generate_query(const wordlist_t *wordlist, const wordlist_t *dictionary)
{
  // some parameters that affect query generation
  const int max_words = 6;            // generate 1..max_words
  const float or_probability = 0.3;   // P(OR between two words)
  const float and_probability = 0.2;  // P(AND between two words)
  const float dict_probability = 0.2; // P(draw from dict instead of wordlist)

  int qwords = random() % max_words + 1; // number of words in query
  for (int qw = 0; qw < qwords; qw++) {
    // draw a word either dictionary or wordlist
    if ((random() % 100) < (dict_probability * 100)) {
      printf("%s ", dictionary->words[random() % dictionary->nwords]);
    } else {
      printf("%s ", wordlist->words[random() % wordlist->nwords]);
    }

    // last word?
    if (qw < qwords-1) {
      // which operator to print?
      int op = random() % 100;
      if (op < (and_probability * 100)) {
        printf("AND ");
      }
      else if (op < (and_probability * 100 + or_probability * 100)) {
        printf("OR ");
      }
    }
  }
  printf("\n");
}
