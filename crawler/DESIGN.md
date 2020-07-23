# TSE Crawler Design Spec

Recall the [Crawler Requirements Spec](REQUIREMENTS.md); the **crawler** crawls a website and retrieves webpages starting with a specified URL.
It parses the initial webpage, extracts any embedded URLs and retrieves those pages, and crawls the pages found at those URLs, but limits itself to `maxDepth` hops from the seed URL and to URLs that are 'internal' to the designated CS50 server.
When the crawler process is complete, the indexing of the collected documents can begin.

A Design Spec should contain several sections:

* User interface
* Inputs and Outputs
* Functional decomposition into modules
* Pseudo code for logic/algorithmic flow
* Dataflow through modules
* Major data structures
* Testing plan

Let's look through each.

### User interface

The crawler's only interface with the user is on the command-line; it must always have three arguments.

```
crawler seedURL pageDirectory maxDepth
```

For example:

``` bash
$ crawler http://old-www.cs.dartmouth.edu/index.html ../data/ 2
```

### Inputs and outputs

Input: the only inputs are command-line parameters; see the User Interface above.

Output: We save each explored webpage to a file, one file per page.
We use a unique document ID as the file name, for document IDs 1, 2, 3, 4, and so forth.
Within a file, we write

 * the page URL on the first line,
 * the depth of the page (where the seed is depth 0) on the second line,
 * the page contents, beginning on the third line.

### Functional decomposition into modules

We anticipate the following modules or functions:

 1. *main*, which parses arguments and initializes other modules
 2. *crawler*, which loops over pages to explore, until the list is exhausted
 3. *pagefetcher*, which fetches a page from a URL
 4. *pagescanner*, which extracts URLs from a page and processes each one
 4. *pagesaver*, which outputs a page to the the appropriate file

And some helper modules that provide data structures:

 1. *bag* of pages we have yet to explore
 4. *hashtable* of URLs we've seen so far

### Pseudo code for logic/algorithmic flow

The crawler will run as follows:

1. execute from a command line as shown in the User Interface
2. parse the command line, validate parameters, initialize other modules
3. make a *webpage* for the `seedURL`, marked with depth=0
4. add that page to the *bag* of webpages to crawl
5. add that URL to the *hashtable* of URLs seen
3. while there are more webpages to crawl,
	5. extract a webpage (URL,depth) item from the *bag* of webpages to be crawled,
	4. pause for at least one second,
	6. use *pagefetcher* to retrieve the HTML for the page at that URL,
	5. use *pagesaver* to write the webpage to the `pageDirectory` with a unique document ID, as described in the Requirements.
	6. if the webpage depth is < `maxDepth`, explore the webpage to find links:
		7. use *pagescanner* to parse the webpage to extract all its embedded URLs;
		7. for each extracted URL,
			8. 'normalize' the URL (see below)
			9. if that URL is not 'internal' (see below), ignore it;
			9. try to insert that URL into the *hashtable* of URLs seen
				10. if it was already in the table, do nothing;
				11. if it was added to the table,
					12. make a new *webpage* for that URL, at depth+1
					13. add the new webpage to the *bag* of webpages to be crawled

**normalize the URL** means to convert it into a clean, canonical form.
As one simple example, 
`Http://Old-WWW.CS.Dartmouth.edu//index.html`
becomes
`http://old-www.cs.dartmouth.edu/index.html`.

**internal** means the URL stays within the CS50 playground `http://old-www.cs.dartmouth.edu/`.

A good implementation will not necessarily encode all the above code in a single, deeply-nested function; part of the Implementation Spec is to break the pseudocode down into a cleanly arranged set of functions.

Notice that our pseudocode says nothing about the order in which it crawls webpages; since it presumably pulls them out of a *bag*, and a *bag* abstract data structure explicitly denies any promise about the order of items removed from a bag, we can't expect any particular crawl order.
That's ok.
The result may or may not be a Breadth-First Search, but for the crawler we don't care about the order as long as we explore everything within the `maxDepth` neighborhood.

The crawler completes and exits when it has nothing left in its bag - no more pages to be crawled.
The maxDepth parameter indirectly determines the number of pages that the crawler will retrieve.


### Dataflow through modules

 1. *main* parses parameters and passes them to the crawler.
 2. *crawler* uses a bag to track pages to explore, and hashtable to track pages seen; when it explores a page it gives the page URL to the pagefetcher, then the result to pagesaver, then to the pagescanner.
 3. *pagefetcher* fetches the contents (HTML) for a page from a URL and returns.
 4. *pagesaver* outputs a page to the appropriate file.
 4. *pagescanner* extracts URLs from a page and returns one at a time.

### Major data structures

Three helper modules provide data structures:

 1. *bag* of page (URL, depth) structures
 2. *set* of URLs (indirectly used by hashtable)
 4. *hashtable* of URLs

### Testing plan

*Unit testing*.  A small test program to test each module to make sure it does what it's supposed to do.

*Integration testing*.  Assemble the crawler and test it as a whole.
In each case, examine the output files carefully to be sure they have the contents of the correct page, with the correct URL, and the correct depth.
Ensure that no pages are missing or duplicated.
Print "progress" indicators from the crawler as it proceeds (e.g., print each URL explored, and each URL found in the pages it explores) so you can watch its progress as it runs.

0. Test the program with various forms of incorrect command-line arguments to ensure that its command-line parsing, and validation of those parameters, works correctly.

0. Test the crawler with a `seedURL` that points to a non-existent server.

0. Test the crawler with a `seedURL` that points to a non-internal server.

0. Test the crawler with a `seedURL` that points to a valid server but non-existent page.

1. Crawl a simple, closed set of cross-linked web pages.
Ensure that some page(s) are mentioned multiple times within a page, and multiple times across the set of pages.
Ensure there is a loop (a cycle in the graph of pages).
In such a little site, you know exactly what set of pages should be crawled, at what depths, and you know where your program might trip up.

2. Point the crawler at a page in that site, and explore at depths 0, 1, 2, 3, 4, 5.
Verify that the files created match expectations.

2. Repeat with a different seed page in that same site.
If the site is indeed a graph, with cycles, there should be several interesting starting points.

3. Point the crawler at our Wikipedia playground.
Explore at depths 0, 1, 2.
(It takes a long time to run at depth 2 or higher!) Verify that the files created match expectations.

5. When you are confident that your crawler runs well, test it on a part of our playground or with a greater depth - but be ready to kill it if it seems to be running amok.
