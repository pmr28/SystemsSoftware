/*
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *
 *  1. Each load/store can cause at most one cache miss. (I examined the trace,
 *  the largest request I saw was for 8 bytes).
 *
 *  2. Instruction loads (I) are ignored, since we are interested in evaluating
 *  data cache performance.
 *
 *  3. data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus an possible eviction.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * IMPORTANT: This is crucial for the driver to evaluate your work.
 *
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

//#define DEBUG_ON
#define ADDRESS_LENGTH 64

/* Type: Memory address */
typedef unsigned long long int mem_addr_t;

/*
 * Data structures to represent the cache we are simulating
 *
 * TODO: Define your own!
 *
 * E.g., Types: cache, cache line, cache set
 * (you can use a counter to implement LRU replacement policy)
 */

 typedef struct cacheBlock {
     int valid;
     int tag;
     int lru_count;
 } Block;

 Block **cache;

/* Globals set by command line args */
int verbosity = 0; /* print trace if set */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
char* trace_file = NULL;

/* Derived from command line args */
int S; /* number of sets */
int B; /* block size (bytes) */

/* Counters used to record cache statistics */
int miss_count = 0;
int hit_count = 0;
int eviction_count = 0;
unsigned long long int lru_counter = 1;



/*
 * initCache - Allocate memory (with malloc) for cache data structures (i.e., for each of the sets and lines per set),
 * writing 0's for valid and tag and LRU
 *
 * TODO: Implement
 *
 */
void initCache()
{
  cache = malloc(S * sizeof(Block)); // allocate memory for each set (row)
  if (cache) {
    for (int i = 0; i < S; i++) {
      cache[i] = malloc(E * sizeof(Block)); // allocate a Block for each each line (column) in the set
      if (cache[i]) {
        for (int j = 0; j < E; j++) { // set all the Blocks to default values
          cache[i][j].valid = 0;
          cache[i][j].tag = 0;
          cache[i][j].lru_count = 0;
        }
      }
    }
  }
}


/*
 * freeCache - free allocated memory
 *
 * This function deallocates (with free) the cache data structures of each
 * set and line.
 *
 * TODO: Implement
 */
void freeCache()
{
  for (int i = 0; i < S; i++) {
    free(cache[i]); // free each set
  }
  free(cache); // free the cache itself
}


/*
 * accessData - Access data at memory address addr
 *   If it is already in cache, increase hit_count
 *   If it is not in cache, bring it in cache, increase miss count.
 *   Also increase eviction_count if a line is evicted.
 *
 * TODO: Implement
 */
void accessData(mem_addr_t addr)
{
  bool found = false;
  bool isSpace = false;
  int set = (addr >> b) & (S - 1); // to get the set shift by b and use a mask of all 1's
  unsigned tag = addr >> (s + b); // the tag is all the leftover bits after shifting by s + b

  for (int i = 0; i < E; i++) { // traverse through the lines of the set
    if (cache[set][i].valid == 1 && cache[set][i].tag == tag) { // if valid and tag matches
      found = true;
      cache[set][i].lru_count = lru_counter;
      hit_count++; // we hit and update the counter accordingly
      break;
    }
  }

  if (!found) {
    miss_count++; // we missed
    for (int j = 0; j < E; j++) { // traverse through the lines in the set
      if(cache[set][j].valid == 0) { // check if there is space
        isSpace = true;
        cache[set][j].valid = 1; // insert the values
        cache[set][j].tag = tag;
        cache[set][j].lru_count = lru_counter;
        break;
      }
    }

    if (!isSpace) { // if there was no space we must evict a Block
      eviction_count++;
      int least_lru = cache[set][0].lru_count;
      int least_index = 0;

      for (int k = 1; k < E; k++) { // traverse through the lines of the set
        if (cache[set][k].lru_count < least_lru) { // find the Block with the smallest lru_count
          least_lru = cache[set][k].lru_count;
          least_index = k;
        }
      }

      cache[set][least_index].tag = tag; // change the values to evict it and insert the new Block
      cache[set][least_index].lru_count = lru_counter;
    }
  }
  lru_counter++; // each time we make an access increment the global counter
}


/*
 * replayTrace - replays the given trace file against the cache
 *
 * This function:
 * - opens file trace_fn for reading (using fopen)
 * - reads lines (e.g., using fgets) from the file handle (may name `trace_fp` variable)
 * - skips lines not starting with ` S`, ` L` or ` M`
 * - parses the memory address (unsigned long, in hex) and len (unsigned int, in decimal)
 *   from each input line
 * - calls `access_data(address)` for each access to a cache line
 *
 * TODO: Implement
 *
 */
void replayTrace(char* trace_fn)
{
  FILE *tFile;
  tFile = fopen(trace_fn, "r");

  char str[100];
  char operation[2];
  unsigned long address;
  unsigned int len;

  while (fgets(str, sizeof(str), tFile)) { // while !EOF
    sscanf(str, "%s %lx, %u", operation, &address, &len);
    if (strcmp(operation, "I") != 0) { // if operation is I skip the line
      accessData(address);
      if (strcmp(operation, "M") == 0) { // if operation is M we must access twice
        accessData(address);
      }
    }
  }

  fclose(tFile);
}

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 *
 * !! DO NOT MODIFY !!
 *
 * printSummary - Summarize the cache simulation statistics. Student cache simulators
 *                must call this function in order to be properly autograded.
 */
void printSummary(int hits, int misses, int evictions)
{
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

/*
 * main - Main routine
 */
int main(int argc, char* argv[])
{
    char c;

    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c){
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

    /* Compute S, E and B from command line args */
    S = (unsigned int) pow(2, s);
    B = (unsigned int) pow(2, b);

    /* Initialize cache */
    initCache();

#ifdef DEBUG_ON
    printf("DEBUG: S:%u E:%u B:%u trace:%s\n", S, E, B, trace_file);
#endif

    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_count, miss_count, eviction_count);

    return 0;
}
