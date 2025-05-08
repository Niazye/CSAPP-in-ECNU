#include "cachelab.h"
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned int Uint;
typedef struct
{
	/* data field in cache line is not necessary in this simulator */
	char valid;
	Uint tag;
	Uint LRU;
} CacheLine;

void print_help();
void parse_addr(unsigned long addr, Uint num_s, Uint num_b, Uint *tag, Uint *index, Uint *offset);
int access_data(CacheLine **cache_begin, Uint tag, Uint index, Uint num_E, Uint *line);
CacheLine **initCache(Uint num_s, Uint num_E);
void freeCache(CacheLine **cache_begin, Uint num_s, Uint num_E);

int main(int argc, char *argv[])
{
	/* Used for parse command line arguments */
	char opt;
	char *file_addr;
	int verbose = 0;

	/* Cache Variables */
	Uint num_s = 0, num_b = 0, num_E = 0;
	CacheLine **cache_begin;

	/* Trace Variables */
	FILE *trace_file;
	char instruction;
	unsigned long addr;
	Uint tag, index, offset, value;

	/* Counters */
	Uint hits = 0, misses = 0, evictions = 0;
	Uint instruction_count = 0;

	/* Used for simulating*/
	int instruction_result;
	Uint line_idx;

	/* Parse command line arguments */
	/* Get cache parameters and help verbose flag */
	while ((opt = getopt(argc, argv, "vhs:E:b:t:")) != -1)
	{
		switch (opt)
		{
		case 'h':
			print_help();
		case 'v':
			verbose = 1;
			break;
		case 's':
			num_s = atoi(optarg);
			break;
		case 'E':
			num_E = atoi(optarg);
			break;
		case 'b':
			num_b = atoi(optarg);
			break;
		case 't':
			file_addr = optarg;
			break;
		default:
		}
	}
	if (num_b == 0 || num_E == 0 || num_s == 0 || file_addr == NULL)
	{
		printf("%s: Missing required command line arguments.\n", argv[0]);
		print_help();
		exit(1);
	}
	trace_file = fopen(file_addr, "r");
	if (trace_file == NULL)
	{
		printf("%s: No such file or directory.\n", file_addr);
		exit(1);
	}

	/* Initialize cache */
	/* Use 2-dimensional array to represent cache */
	/* Array line for cache set and array column for cache line in set*/
	cache_begin = initCache(num_s, num_E);

	/* Read trace file and simulate cache */
	/* The behavior of laod_data and store_data are identical in this simulator */
	/* So just use single access_data function to simulate them */
	/* As to modify_data, just call access_data twice */
	while (fscanf(trace_file, " %c %lx,%x", &instruction, &addr, &value) != EOF)
	{
		parse_addr(addr, num_s, num_b, &tag, &index, &offset);
		instruction_count++;
		if (verbose)
			printf("%c %lx,%x", instruction, addr, value);
		switch (instruction)
		{
		case 'M':
			instruction_result = access_data(cache_begin, tag, index, num_E, &line_idx);
			if (instruction_result == 0)
			{
				if (verbose)
					printf(" hit");
				hits++;
			}
			else if (instruction_result == 1)
			{
				if (verbose)
					printf(" miss");
				misses++;
			}
			else
			{
				if (verbose)
					printf(" miss eviction");
				misses++;
				evictions++;
			}
		case 'L':
		case 'S':
			instruction_result = access_data(cache_begin, tag, index, num_E, &line_idx);
			if (instruction_result == 0)
			{
				if (verbose)
					printf(" hit");
				hits++;
			}
			else if (instruction_result == 1)
			{
				if (verbose)
					printf(" miss");
				misses++;
			}
			else
			{
				if (verbose)
					printf(" miss eviction");
				misses++;
				evictions++;
			}
			cache_begin[index][line_idx].LRU = instruction_count;
			if (verbose)
				printf("\n");
			break;
		default:
		}
	}
	freeCache(cache_begin, num_s, num_E);
	fclose(trace_file);
	
	printSummary(hits, misses, evictions);

	return 0;
}

void parse_addr(unsigned long addr, Uint num_s, Uint num_b, Uint *tag, Uint *index, Uint *offset)
{
	/* Parse address to get tag, index, and offset */
	*index = (addr >> num_b) & ((1 << num_s) - 1);
	*offset = addr & ((1 << num_b) - 1);
	*tag = addr >> (num_b + num_s);
	return;
}
CacheLine** initCache(Uint num_s, Uint num_E)
{
	/* Initialize cache */
	/* Use 2-dimensional array to represent cache */
	/* Array line for cache set and array column for cache line in set*/
	int i = 0, j = 0;
	Uint num_S = 1 << num_s;
	CacheLine **cache_begin = malloc(num_S * sizeof(CacheLine *));
	for (i = 0; i < num_S; i++)
	{
		cache_begin[i] = (CacheLine *)malloc(num_E * sizeof(CacheLine));
		for (j = 0; j < num_E; j++)
		{
			cache_begin[i][j].valid = 0;
			cache_begin[i][j].tag = 0;
			cache_begin[i][j].LRU = 0;
		}
	}
	return cache_begin;
}
void freeCache(CacheLine **cache_begin, Uint num_s, Uint num_E)
{
	/* Free cache */
	int num_S = 1 << num_s;
	int i = 0;

	for (i = 0; i < num_S; i++)
	{
		free(cache_begin[i]);
	}
	free(cache_begin);
	return;
}
int access_data(CacheLine **cache_begin, Uint tag, Uint index, Uint num_E, Uint *line)
{
	/* Access data in cache */
	int i = 0;
	int to_evict = 0;
	/* Search cache set for valid line with matching tag */
	for (i = 0; i < num_E; i++)
	{
		if (cache_begin[index][i].valid == 1)
		{
			if (cache_begin[index][i].tag == tag)
			{
				*line = i;
				/* if found, return hit */
				return 0;
			}
		}
	}

	/* If not found, search for line to evict */
	/* Find line with lowest LRU value */
	for (i = 0; i < num_E; i++)
	{
		if (cache_begin[index][i].LRU < cache_begin[index][to_evict].LRU)
		{
			to_evict = i;
		}
	}
	*line = to_evict;
	cache_begin[index][to_evict].valid = 1;
	cache_begin[index][to_evict].tag = tag;

	/* If the LRU value of the line to evict is 0 */
	/* It means that line is empty, No data in it */
	if (cache_begin[index][to_evict].LRU == 0)
	{
		/* If so */
		return 1;
	}

	/* Else, evict the line and return miss and eviction */
	return -1;
}
void print_help()
{
	printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
	printf("Options:\n");
	printf("  -h         Print this help message.\n");
	printf("  -v         Optional verbose flag.\n");
	printf("  -s <num>   Number of set index bits.\n");
	printf("  -E <num>   Number of lines per set.\n");
	printf("  -b <num>   Number of block offset bits.\n");
	printf("  -t <file>  Trace file.\n");
	printf("\n");
	printf("Examples:\n");
	printf("  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
	printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
	exit(0);
	return;
}
