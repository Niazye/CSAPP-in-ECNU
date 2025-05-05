#include "cachelab.h"
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef unsigned long Ulong;
typedef unsigned int Uint;
void print_help();
void analyze_addr(Ulong addr, Ulong num_s, Ulong num_t, Ulong num_b, Ulong* tag, Ulong* index, Ulong* offset);
int main(int argc, char* argv[]) {
	argc = 9;
	argv[0] = "./PRO";
	argv[1] = "-s";
	argv[2] = "8";
	argv[3] = "-E";
	argv[4] = "2";
	argv[5] = "-b";
	argv[6] = "4";
	argv[7] = "-t";
	argv[8] = "traces/yi.trace";

	Uint opt;
	FILE* trace_file;
	char* file_addr;
	Uint num_s = 0, num_b = 0, num_m = 64, num_t;
	Uint num_S, num_B, num_E = 0, num_C;
	Uint tot_size;
	void* cache_begin;
	char instrtuction;
	Ulong addr, value;
	Ulong tag, offset, index;
	while ((opt = getopt(argc, argv, "vhs:E:b:t:")) != -1) {
		switch (opt) {
			case 'h':
				print_help();
				exit(0);
			case 'v':
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
	num_B = 1 << num_b;
	num_t = num_m - num_s - num_b;
	num_C = num_S * num_E * num_B;

	tot_size = num_S * num_E * (1 + num_t + num_B);
	cache_begin = malloc(tot_size);
	while(fscanf(trace_file, "%c %lx,%lx", &instrtuction, &addr, &value)) {
		analyze_addr(addr, num_s, num_t, num_b, &tag, &offset, &index);
		switch(instrtuction) {
			case 'L':

				break;
			case 'S':
				break;
			case 'M':
				break;
			default:
		}

	}

	printf("s = %d, E = %d, b = %d, trace file = %s\n", num_s, num_E, num_b, file_addr);
    	//printSummary(0, 0, 0);

	fclose(trace_file);
	return 0;
}

void analyze_addr(Ulong addr, Ulong num_s, Ulong num_t, Ulong num_b, Ulong* tag, Ulong* index, Ulong* offset) {
	*offset = addr & ((1 << num_b) - 1);
	*tag = (addr >> num_b) & ((1 << num_t) - 1);
	*index = addr >> (num_b + num_t);
	return;
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
