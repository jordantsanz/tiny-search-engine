# TSE Querier Design Spec
Recall the [Querier Requirements Spec](REQUIREMENTS.md); the **querier** queries indexer output and determines the final score of each document as related to how relevant it is to the query, and prints this output into standard output. It parses an initial indexFile from the `indexFile` provided and loads an `index_t` index module from that document, then reads the words of the query given and calculates the score for each document. It will then print any documents with a score greater than 0, ranked by score, with the url printed beside it. It will then prompt for a new query.

### User Interface

`querier` has a two-part user interface. It first has a user interface to run the actual program. The command-line input to start the program should look like the following:

```
./querier pageDirectory indexFile
```

Where ./querier is the program, `pageDirectory` is the path of a crawler-produced directory, and `indexFile` is the path of the `indexer`-produced file from that `crawler`-produced directory. 

For example:

```bash
$ ./querier ../data/test1 ../data/indexFiles/indexFile1
```

The second user interface is when the program is running, and `querier` prompts the user to input a query. It will say this by printing "Query?". To enter a query, simply write any string of words and press the enter key. It will then calculate this query's results, print the results, and prompt for another query. Entering a query can be done as such:

```
Query?
```

```
this is my query (Enter)
```

Where (Enter) stands for the user pressing the enter key. To end `querier`, the user should give an EOF command, or press `cntrl + D`. 

To learn more about what kinds of queries are allowed, go to the Assumptions section in README.md.

### Inputs and Outputs

Input: The first inputs given to `querier` are the command-line parameters; see the User Interface above. The inputs following are then each `query` given by the user. See the User Interface above for more information on how to give a query, and see the Assumptions section in README.md to see the rules for queries allowed.

Output: Assuming that a query meets all of the requirements to run, we save all of the contents of a `counters_t` counters module to standard output. The format of this output will be one of two things:

```
Matched __ results (ranked):
``` 
Where the space represents the number of docuemnts found, with then each document following looking in the format of:
```
Score: ____ DocID: _____ URL: _____
```
With the blanks representing the overall score, the document's ID, and the URL of the document's contents, respectively. Each document will be ranked in order of highest score to lowest score, with the highest score being most relevant to the search query.

For example, a possible output would be:
```
Score: 2 DocID: 4 URL: www.hello.com
```
This example would have found one document only that matched the query, giving it an overall score of 2. That document's contents would have come from the URL "www.hello.com". 

The other possible output, assuming the query met all of the requirements to run, is that no documents were found, in which the output will be:

```
No documents found.
```

Both of these outputs will then be followed by "Query?" when `querier` prompts for a new query. 

### Functional decomposition into modules
We anticipate the following modules or functions:

1. *main*, which parses arguments and verifies them
2. *querier*, which prompts for queries and calls the necessary functions to calculate the score of each document for a query
3. *parseQuery*, which determines the query's results by calling all of the next modules
4. *separateQuery*, which separates the input string by the user into an array of words and checks to make sure each word is valid
5. *printCleanedQuery*, which normalizes each word and checks to make sure the words "and" and "or" are in valid places in the query
6. *calculateQuery*, which calculates the score of each document by working through the array of words and determining if an *and* query should take place or an *or* query
7. *orQuery*, which sums the scores of the documents in two counters
8. *andQuery*, which takes the minimum score of documents in two counters
9. *orderResults*, which takes the final counter holding the scores of all documents and creates an array of *documentResult_t* objects, sorted from highest to lowest score
10. *printResults*, which prints the sorted array of documents into standard output


And some helper modules that provide data structures:
1. *index* of countersets that keep track of the counts of words in documents
2. *counter* of documentID and score pairs that keeps track of the running score of each document
3. *twocts* that holds two counters to merge and intersect their scores
4. *documentResult* that holds a documentID and a count to order and print as output after all documents are scored

### Pseudocode for logic/algorithmic flow

The querier will run as follows:

1. execute from a command line as shown in the User Interface
2. parse the command line, validate parameters, initialize other modules
3. prompt the user to enter in a query. For every query entered:
    4. create a counters_t object to keep track of the scores of documents
    5. load in an `index_t` module from the `indexFilename` provided
    6. separate the user's query into words and validate that they are all in the valid format by:
        7. moving a front index to the first index in the input string with a letter
        8. moving an ending index to one index after the last index with a letter in the string
        9. inserting a end-of-string character at the ending index if one does not already exist
        10. moving a front index and another index throughout the query, and inserting an end-of-string character at the end of each word in the query
        11. adding a pointer to the word formed in the query to an array of string pointers
        12. returning the array of string pointers with each word in the query put into the array
    13. printing the array of words by:
        14. normalizing each word in the array
        15. verifying that 'and' or 'or' is not at the start of the query or the end of the query
        16. printing each word in the query with "Query found: " preceeding it
    17. creating an `orseq`, or "or sequence" `counters_t` object to keep track of the score of each document, and calculating the score of each document for the given query by:
        18. trying to find a `counters_t` object in the `index_t` module associated with the first word in the query, and creating a blank `counters_t` object if not; this `counters_t` object will be referred to as `andseq`, or the "and sequence"
        19. creating a variable to keep track of any found conjunction ("and" or "or")
        20. creating another `counters_t` variable to keep track of another word in the query, if any
        21. For each word in the query:
            22. If the word is 'and':
                23. Checking if there is already a conjunction, and if there is, exiting because two conjunctions are in a row
                24. setting the current `conjunction` as "and" and continuing to the next word
            25. If the word is 'or':
                26. Checking if there is already a conjunction, and if there is, exiting because two conjunctions are in a row
                27. setting the current `conjunction` as "or" and continuing to the next word
            28. Checking if the current andsequence is `NULL`, and if it is:
                29. Trying to find the current word in the `index_t` module and returning the `counters_t` object associated with it, setting that `counters_t` counter to `andseq`
                30. If there is no `counters_t` counter associated with the current word, creating a blank `counters_t` object and setting it as `andseq`
                31. Setting the current conjunction to `NULL` (since `andseq` will only be `NULL` if an `or` query just happened, and thus, there should not be an `and` after this `or`; not setting it to `NULL` until now ensures this), and continuing to the next word
            32. If `conjunction` is `NULL` or the current `conjunction` is `and`:
                33. Finding the current word in `index_t` and returning the `counters_t` counter associated with it and setting that as `counters_2`
                34. If there is not a `counters_2` object, making a blank `counters_2` object
                35. Running the `and` intersect query
                36. Setting `conjunction` to `NULL` and `counters_2` to NULL, and continuing to the next word
            37. If the current conjunction is `or`:
                38. Running an `or` query
                39. Setting `andseq` to `NULL`
        40. Checking to see if `andseq` has anything left to merge, and if it does, merging it
        41. Ordering the resulting documents found by:
            42. Creating an array of `documentResult_t` objects
            43. For every documentID/score pair in `orseq`:
                45. Creating a `documentResult_t` object
                46. Running an insertion sort algorithm and inserting it into the array from highest count to lowest count
        47. Printing the array of `documentResult_t` objects by looping through the array and printing:
            48. The score of the document
            49. The document ID
            50. The url of the document by first:
                51. Building a `filename` string that has the `pageDirectory` followed by a `/` followed by the documentID
                52. Opening that document and reading the first line of the URL
                53. Returning that URL string to be printed
    54. Ending the program by giving an EOF sequence
55. Returning successfully.

More detailed pseudocode can be found in the IMPLEMENTATION.md file.

**normalize the word** means to convert a word into a strictly lowercase format.

### Dataflow through modules

For querier:

1. *main* parses parameters and passes them to querier.
2. *querier* reads in a string of input from the user and passes it to `parseQuery` along with the pageDirectory and the indexFilename.
3. *parseQuery* creates an `orseq` counter and loads in the `index_t` module from the `indexFilename` file. It also creates a blank array of string pointers. It then passes the user's input string and the blank array to `separateQuery`.
4. *separateQuery* fills the blank array and returns the number of indices in the array to *parseQuery*.
5. *parseQuery* sends the filled array of words and the number of indices in the array to *printCleanedQuery*.
6. *parseQuery* then sends the filled array, the number of indices in the array, the `index_t` module, and the blank `orseq` counters object to *calculateQuery*.
7. *calculateQuery* fills the blank `orseq` counters object with the relevant documents to the query and the scores of each document.
8. *parseQuery* then sends the filled `orseq` counters object to *orderResults* along with the `pageDirectory`.
9. *orderResults* creates an empty array of `documentResult_t` objects and passes the blank array and the filled `orseq` counters object to *counters_iterate* along with the helper function *createDocumentHelper*, which creates a `documentResult_t` object for each document and puts it into the results array.
10. *orderResults* then sends the filled array to *insertionSort*, which sorts the array from highest to lowest score.
11. *orderResults* then sends the sorted and filled array, the `pageDirectory`, and the number of documents in the array to *printResults*.
12. *printResults* then prints each document score and document ID in the array, as well as the URL of each document by passing the documentID and the `pageDirectory` to *findURL*. 

### Major data structures

Four helper modules provide data structures:

1. *index* of word and documentID/count pairings
2. *hashtable* of word and counter pairings, used indirectly by *index*
3. *set* of words used indirectly by hashtable
4. *counters* of documentID and scores for the documents. Also *counters* of documetIDs and word frequencies used by *index*
5. *documentResult* that keeps track of the documentID and the score of that document
6. *twocts* that holds two counters to find the scores of the documents between them

### Testing plan

0. Test various command-line errors in querier to ensure that its command-line parsing, and the validation of those parameters, works correctly.

0. Test the querier with no command-line arguments, not including the program itself.

0. Test the querier with one command-line argument, not including the program itself.

0. Test the querier with three command-line arguments, not including the program itself.

0. Test the querier when the directory provided is not a crawler produced directory.

0. Test the querier for when the indexFile provided is not a real file.

1. Test queries that have non-words in them, like symbols and numbers.

2. Test queries that have conjunctions at the start and end of the queries.

3. Test queries that have two conjunctions back to back in the query.

4. Test queries that provide no input.

4. Test queries that are made up of only spaces.

5. Test queries that have words that do not exist in the indexFile.

6. Test queries with single words in uppercase, lowercase, and a mix of uppercase and lowercase format.

7. Test queries that have two words that are both in the index, with no conjunction in between them and with an "and" in between them.

8. Test queries with two words that both exist in the index, with the "or" conjunction in between them.

9. Test queries with two words when one word is not in the index, with "and", "or", and nothing in between them. 

10. Test queries with multiple conjunctions to test that "and" has precedence over "or".

11. Test queries with multiple conjunctions to test that "and" has precendence over "or", even when words provided are not found in the index.

12. Test queries with varying lengths of input and various conjunctions.

13. Test queries with varying inputs, but with many spaces at the start, at the end, and in between words of the query. 