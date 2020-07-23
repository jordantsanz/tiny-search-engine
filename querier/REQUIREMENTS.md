---
layout: page
title: TSE Querier Requirements Spec
---

The TSE Querier is a standalone program that reads the index file produced by the TSE Indexer, and page files produced by the TSE Querier, and answers search queries submitted via stdin.

The querier **shall**:

1. execute from a command line with usage syntax
   * `./querier pageDirectory indexFilename`
   * where `pageDirectory` is the pathname of a directory produced by the Crawler, and
   * where `indexFilename` is the pathname of a file produced by the Indexer.

2. validate its command-line arguments:
	* `pageDirectory` is the pathname for an existing directory; if it cannot be verified to be a directory produced by the Crawler, the indexer shall print an error message and exit non-zero. 
	* `indexFilename` is the pathname of a readable file; if it is not possible to open it for reading, the querier should print an error message and exit non-zero.

2. load the index from `indexFilename` into an internal data structure.

3. read search queries from stdin, one per line, until EOF.
	4. clean and parse each query according to the *syntax* described below.
	5. if the query syntax is somehow invalid, print an error message, do not perform the query, and prompt for the next query.
	5. print the 'clean' query for user to see.
	5. use the index to identify the set of documents that *satisfy* the query, as described below.
	6. if the query is empty (no words), print nothing.
	6. if no documents satisfy the query, print `No documents match.`
	7. otherwise, rank the resulting set of documents according to its *score*, as described below, and print the set of documents in decreasing rank order; for each, list the score, document ID and URL.
(Obtain the URL by reading the first line of the relevant document file from the `pageDirectory`.)
	8. Exit with zero status when EOF is reached on stdin.

The querier *may* assume that the input directory and files follow the designated formats:

* `pageDirectory` has files named 1, 2, 3, ..., without gaps.
* The content of files in `pageDirectory` follow the format as defined in the specs; thus your code (to read the files) need not have extensive error checking.
* The content of the file named by `indexFilename` follows our [Index file format]({{site.labs}}/lab5/REQUIREMENTS.html#index-file-format); thus your code (to recreate an index structure by reading a file) need not have extensive error checking.

## Queries

The specs above indicate that you read one query per line, parse the query according to its *syntax*, determine which documents *satisfy* the query, and determine *score* for each document satisfying the query.
We define each term below.

### Syntax {#syntax}

Queries are a sequence of words, with optional boolean operators ('and', 'or'), where 'and' has higher precedence than 'or'.

First, we ignore blank lines (i.e., empty or nothing but spaces).

Second, we translate all upper-case letters on the input line into lower-case.

Then, we use [Backus-Naur Form](https://en.wikipedia.org/wiki/Backus–Naur_Form) for describing the query syntax; if you have not heard of BNF before, you're sure to see it in future CS classes (or software specs!).

```
query       ::= <andsequence> [or <andsequence>]...
andsequence ::= <word> [ [and] <word>]...
```
and `word` is a sequence of one or more (lower-case) letters.

Think of the query as a sequence of *tokens*, each a `word` or a literal ('and', 'or'), with space(s) between tokens, and optional space at the beginning or end of the line.
Note, too, that the literal 'and' may be left out and is thus implicit.

Notice how the structure of the syntax indicates that 'and' has higher precedence (binds more tightly) than 'or'.
A query is an OR'd sequence of AND-sequences, or for those of you familiar with logic, a *disjunction* of *conjunctions*.

> By the way, this is very similar to another grammar you know well: arithmetic.
>
>```
>expression ::= <product> [ + <product>]...
>product  ::= <number> [ * <number>]...
>```
>and `number` is a sequence of one or more digits.
>In an equation like `4 + 5 * 6`, the answer is `4 + (5 * 6) = 34`, not `(4 + 5) * 6 = 54`, because `*` has higher precedence than `+`.
>See [Lecture notes]({{site.lectures}}/querier/).

Single-word examples (all equivalent):

```
 Dartmouth
 dartmouth
     dartmouth
```

Two-word examples (all equivalent):

```
 Dartmouth College
 dartmouth college
 dartmouth and college
    dartmouth    AND    College
```

The following two examples are different:

```
Dartmouth College AND computer science
Dartmouth College or computer science
```

There is an implicit 'and' between 'Dartmouth' and 'College' and between 'computer' and 'science'.
(Capitalization does not matter.) Thus, the above two queries are equivalent to

```
Dartmouth AND College AND computer AND science
Dartmouth AND College or computer AND science
```

The first is a single `andsequence` (a single conjunction); the second is a combination of a first `andsequence` and a second `andsequence` - that is, a disjunction of two conjunctions.

You can combine words more richly, of course:

```
Dartmouth or computer science or programming or Unix or Doug McIlroy
```

The following queries all have syntax errors:

```
and
or
and dartmouth
or dartmouth
dartmouth college or
dartmouth college and
dartmouth college and or computer
dartmouth college and and computer
dartmouth college or and computer
computer science 50
Dartmouth!
Backus-Naur Form
```

because (per the syntax)

 * the literal 'and' must come between `words`, and the literal 'or' must come between `andsequences`, so they cannot appear at the beginning or end of a query,
 * the literals ('and' & 'or') cannot be adjacent
 * characters other than letters or spaces are disallowed.

Example output from the querier for some of the queries above:

```
Query: tse or for breadth or the first 
Matches 3 documents (ranked):
score   1 doc   1: http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/
score   1 doc   3: http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/index.html
score   1 doc   4: http://old-www.cs.dartmouth.edu/~cs50/data/tse/letters/B.html
-----------------------------------------------
Query: and 
Error: 'and' cannot be first
Query: or 
Error: 'or' cannot be first
Query: and dartmouth 
Error: 'and' cannot be first
Query: or dartmouth 
Error: 'or' cannot be first
Query: dartmouth college or 
Error: 'or' cannot be last
Query: dartmouth college and 
Error: 'and' cannot be last
Query: dartmouth college and or computer 
Error: 'and' and 'or' cannot be adjacent
Query: dartmouth college and and computer 
Error: 'and' and 'and' cannot be adjacent
Query: dartmouth college or and computer 
Error: 'or' and 'and' cannot be adjacent
Error: bad character '5' in query.
Error: bad character '!' in query.
Error: bad character '-' in query.
```

### Satisfy

A document *satisfies* a single-word query for `word` if that `word` appears in that document.

A document *satisfies* a conjunction (aka andsequence) `wordA and wordB` if both `wordA` and `wordB` appear in the document.
By induction, a document satisfies `<andsequence> and wordC` if the document satisfies the `andsequence` and `wordC` also appears in the document.

A document *satisfies* a disjunction `wordA or wordB` if either `wordA` and `wordB` appear in the document.
By induction, a document satisfies `<andsequenceA> or <andsequenceB>` if the document satisfies the `andsequenceB` or satisfies `andsequenceB`, or both.

### Score

Because we want to rank the documents satisfying the query, we need to score each document by *how well* it satisfies the document.

The score for a document satisfying a single-word query for `word` is the number of occurrences of `word` in that document.
(Fortunately, that's what your index tells you.)

The score for a document satisfying a conjunction (aka `andsequence`) `wordA and wordB` is the *minimum* of the score for `wordA` and the score for `wordB` on this document.
By induction, the score for a document satisfying `<andsequence> and wordC` is the minimum of the score for the `andsequence` and the score for `wordC` on this document.

The score for a document satisfying a disjunction `wordA or wordB` is the *sum* of the score for `wordA` and the score for `wordB` on this document.
By induction, the score for a document satisfying `<andsequenceA> or <andsequenceB>` is the sum of the score for `andsequenceA` and the score for `andsequenceB` on this document.

For example, consider three documents (D1, D2, D3) and three queries:

|query             | D1                  | D2                   | D3                  |
|:-----------------+:--------------------+:---------------------+:--------------------+
*matches*          | 0 cat, 5 dog, 7 emu | 3 cat, 2 dog, 1 emu  | 3 cat, 4 dog, 0 emu |
cat and dog        | score = 0           | score = 2            | score = 3*          |
cat or  dog        | score = 5           | score = 5            | score = 7*          |
cat and dog or emu | score = 7*          | score = 3            | score = 3           |

where * indicates the highest rank for each query.

