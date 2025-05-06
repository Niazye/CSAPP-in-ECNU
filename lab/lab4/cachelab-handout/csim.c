#include "cachelab.h"
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef unsigned long Ulong;
typedef unsigned int Uint;
typedef struct {
	char valid;
	Ulong tag;
	Ulong data;
	Uint LRU;
} CacheLine;
void print_help();
void analyze_addr(Ulong addr, Ulong num_s, Ulong num_b, Ulong* tag, Ulong* index, Ulong* offset);
int access_data(CacheLine** cache_begin, Ulong tag, Ulong index, Uint num_E, Ulong *line);
int main(int argc, char* argv[]) {/* 
	argc = 9;
	argv[0] = "./csim";
	argv[1] = "-s";
	argv[2] = "4";
	argv[3] = "-E";
	argv[4] = "1";
	argv[5] = "-b";
	argv[6] = "4";
	argv[7] = "-t";
	argv[8] = "traces/trans.trace"; */

	Uint opt;
	FILE* trace_file;
	char* file_addr;
	Uint num_s = 0, num_b = 0;
	Uint num_S, num_E = 0;
	CacheLine** cache_begin;
	char instrtuction;
	Ulong addr, value;
	Ulong tag;
	Ulong index, offset;
	Uint i, j;
	Ulong hits = 0, misses = 0, evictions = 0;
	Ulong instruction_count = 0, line_idx;
	int instruction_result;
	int verbose = 0;
	while ((opt = getopt(argc, argv, "vhs:E:b:t:")) != -1) {
		switch (opt) {
			case 'h':
				print_help();
				exit(0);
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
	if(num_b == 0 || num_E == 0 || num_s == 0 || file_addr == NULL) {
		printf("%s: Missing required command line arguments.\n", argv[0]);
		print_help();
		exit(1);
	}
	trace_file = fopen(file_addr, "r");
	if(trace_file == NULL) {
		printf("%s: No such file or directory.\n", file_addr);
		exit(1);
	}
	num_S = 1 << num_s;
	cache_begin = (CacheLine**) malloc(num_S * sizeof(CacheLine*));
	for(i = 0; i < num_S; i++) {
		cache_begin[i] = (CacheLine*) malloc(num_E * sizeof(CacheLine));
		for(j = 0; j < num_E; j++) {
			cache_begin[i][j].valid = 0;
			cache_begin[i][j].tag = 0;
			cache_begin[i][j].data = 0;
			cache_begin[i][j].LRU = 0;
		}
	}
	while(fscanf(trace_file, " %c %lx,%lx", &instrtuction, &addr, &value) != EOF) {
		analyze_addr(addr, num_s, num_b, &tag, &index, &offset);
		instruction_count++;
		if(verbose) printf("%c %lx,%lx", instrtuction, addr, value);
		switch(instrtuction) {
			case 'M':
				instruction_result = access_data(cache_begin, tag, index, num_E, &line_idx);
				if(instruction_result == 0) {
					if(verbose) printf(" hit");
					hits++;
				}
				else if(instruction_result == 1) {
					if(verbose) printf(" miss");
					misses++;
				}
				else {
					if(verbose) printf(" miss eviction");
					misses++;
					evictions++;
				}
			case 'L':
			case 'S':
				instruction_result = access_data(cache_begin, tag, index, num_E, &line_idx);
				if(instruction_result == 0) {
					if(verbose) printf(" hit");
					hits++;
				}
				else if(instruction_result == 1) {
					if(verbose) printf(" miss");
					misses++;
				}
				else {
					if(verbose) printf(" miss eviction");
					misses++;
					evictions++;
				}
				cache_begin[index][line_idx].LRU = instruction_count;
				if(verbose) printf("\n");
				break;
			default:
		}

	}
	fclose(trace_file);

	
    printSummary(hits, misses, evictions);

	return 0;
}

void analyze_addr(Ulong addr, Ulong num_s, Ulong num_b, Ulong* tag, Ulong* index, Ulong* offset) {
	*index = (addr >> num_b) & ((1 << num_s) - 1);
	*offset = addr & ((1 << num_b) - 1);
	*tag = addr >> (num_b + num_s);
	return;
}
int access_data(CacheLine** cache_begin, Ulong tag, Ulong index, Uint num_E, Ulong *line) {
	int i = 0;
	int to_evict = 0;
	for(i = 0; i < num_E; i++) {
		if(cache_begin[index][i].valid == 1) {
			if(cache_begin[index][i].tag == tag) {
				*line = i;
				return 0;
			}
		}
	}
	for(i = 0; i < num_E; i++) {
		if(cache_begin[index][i].LRU < cache_begin[index][to_evict].LRU) {
			to_evict = i;
		}
	}
	*line = to_evict;
	cache_begin[index][to_evict].valid = 1;
	cache_begin[index][to_evict].tag = tag;
	cache_begin[index][to_evict].data = 0;
	if(cache_begin[index][to_evict].LRU == 0) {
		return 1;
	}
	return -1;
}
void print_help() {
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
	return;
}
