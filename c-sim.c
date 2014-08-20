/*TODO Power of 2 check for input*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "cache.h"

int x = 0;

int cache_hits = 0;
int cache_misses = 0;
int mem_reads = 0;
int mem_writes = 0;

int cache_size = 0;
int block_size = 0;
int num_sets = 0;
int block_bits = 0;
int set_bits = 0;
int wt = 0;
int tag_bits = 0;

char rw = 'z';

int stamp = 0;

int read = 0;
char* addr_16;
char* addr_bin;

/* Prints the results of the program
*/

void printResults(void) {

  printf("Cache hits: %d\nCache misses: %d\nMemory reads: %d\nMemory writes: %d\n", cache_hits, cache_misses, mem_reads, mem_writes);

}

/* Gets command line arguments, creates the cache, and processes each line of the cache 
*/

void main(int argv, char** args) {

if(argv < 6) {
	printf("Not enough arguments!");
}

 if(strcmp("-h",args[1]) == 0) {
    printf("Usage: ./c-sim [-h] <cache size> <assoc> <block size> <write policy> <trace file>\nWhere all integers inputs are powers of 2\n");
    /*Print help menu here*/
    return;
  }

  cache_size = atoi(args[1]);
  block_size = atoi(args[3]);
  
  if(cache_size & (cache_size - 1) != 0 || cache_size < 1) {
	printf("Please enter your cache to be a power of two, and positive.\n");
	return;
  }
  if(block_size & (block_size - 1) != 0 || block_size < 1) {
	printf("Please enter your block size to be a power of two, and positive.\n");
	return;
  }
  
  if(strcmp("wt", args[4]) == 0) {
	wt = 1;
  } else
   if(strcmp("wb",args[4]) == 0)  {
	wt = 0;
  } else {
  printf("Please enter wt for wb for write policy\n");
  return;
  }
  
  FILE* fp;
  
  fp = fopen(args[5], "r");
  
  if(fp == NULL) {
	printf("Error, file not found\n");
	return;
  }
  
  int n = -1;
  if(strcmp("direct",args[2]) == 0) {
	n = 0;
  } else
  if(strcmp("assoc",args[2]) == 0) {
	n = -1;
  } else {
  char buff[10];
  int way = 0;
  sscanf(args[2], "%s:%d", buff, &way);

  
  
  if(way != 0) {
  
	if(n & (n - 1) != 0 || n < 1) {
		printf("Please have your associativity factor be a power of two and positive.\n");
		return;
	}
  
  
  n = way;
  
	}
  
  }
  
  cache* c = createCache(n);
  
  set_bits = log(num_sets) / log(2); 
  block_bits = log(block_size) / log(2);
  tag_bits = 32 - set_bits - block_bits;
  
  int i = 0;
  while(fgetc(fp) != '#') {
  int nothing; char mode; char* addr;
  fseek(fp,-1,SEEK_CUR);
  fscanf(fp, "%x: %c %s\n", &nothing, &mode, addr);
  
  rw = mode;
  
  char* binary;
  binary = processHexToBin(addr);
  
  
  writeToCache(c, binary);
  free(binary);
  
  i++;
  }
	
	fclose(fp);
	printResults();
}

/* Creates the cache, with cache type based on user input 
*/
cache* createCache(int n) {

	cache* p;

	if(n == 0) {
	/*Create a direct cache*/
	p = (cache*)malloc(sizeof(cache));
	
	p->direct = 1;
	p->f_assoc = 0;
	p->n_assoc = 0;
	p->n = 1;

	
	int ns = cache_size / block_size;
	num_sets = ns;
	
	
	Set* sets = malloc(num_sets * sizeof(Set));
	
	Line* ln;
	int i = 0;
	for(i = 0; i < num_sets; i++) {
		ln = (Line*)malloc(sizeof(Line));
		ln->valid = 0;
		ln->dirty = 0;
		ln->tag = malloc(50 * sizeof(char));
		
	
		sets[i].lines = ln;
		
	}
	
	p->sets = sets;


	return p;
	} else
	if(n == -1) {
	/*Create a fully assoc cache*/
	
	p = (cache*)malloc(sizeof(cache));
	
	p->direct = 0;
	p->f_assoc = 1;
	p->n_assoc = 0;
	
	num_sets = 1;
	
	Set* sets = malloc(sizeof(Set));
	p->sets = sets;
	
	int lines = cache_size / block_size;
	
	Line* lns = malloc(lines * sizeof(Line));
	Line* newl;
	int i = 0;
	for(i = 0; i < lines; i++) {
		newl = (Line*)malloc(sizeof(Line));
		newl->valid = 0;
		newl->dirty = 0;
		newl->tag = malloc(50 * sizeof(char));
		strcpy(newl->tag, "0");
		
		newl->y = REALLY_OLD;
		
		lns[i] = *newl;
	}
	
	sets[0].lines = lns;
	
	
	return p;
	} else {
	p = (cache*)malloc(sizeof(cache));
	
	p->direct = 0;
	p->f_assoc = 0;
	p->n_assoc = 1;
	p->n = n;
	
	int setnum = cache_size / (block_size * n);
	num_sets = setnum;
	
	
	
	Set* sets = malloc(num_sets * sizeof(Set));
	int i = 0;
	for(i = 0; i < setnum; i++) {
		Line* lns = malloc(n * sizeof(Line));
		int j = 0;
		for(j = 0; j < n; j++) {
		Line* newl = (Line*)malloc(sizeof(Line));
		newl->valid = 0;
		newl->dirty = 0;
		newl->tag = malloc(50 * sizeof(char)); 
		
		lns[j] = *newl;
		}
	sets[setnum].lines = lns;
	}
	
	
	
	
	return p;
	}

	return p;
}

/* Take a base 10 number and returns a pointer to its base 2 representation as a string
*/
char* processBinToString(int num) {
   
   int digit, mark, count;
   char *p;
 
   count = 0;
   p = (char*)malloc(32+1);
 
   for ( mark = 31 ; mark >= 0 ; mark-- )
   {
      digit = num >> mark;
 
      if ( digit & 1 ) {
         *(p+count) = 1 + '0';
		 
     } else {
         *(p+count) = 0 + '0';
		 }
      count++;
   }
   *(p+count) = '\0';
 
   return  p;




}

/* Processes a string representing a hex number and returns a base 2 representation 
*/
char* processHexToBin(char* input) {


int num = (int)strtol(input, NULL, 0);
char* p = processBinToString(num);



return p;
}


/* Attempts to find a cache hit for the given cache type
*
*
*/
void writeToCache(cache* c, char* addr) {

	if(c->direct == 1) {
	
	writeToDirectCache(c, addr);
	
	} else 
	if(c->f_assoc == 1) {
	
	writeToFullAssoc(c, addr);
	
	} else {
	
	writeToNAssoc(c, addr);
	
	}

}

void writeToFullAssoc(cache* c, char* addr) {



char tag[tag_bits + 1];
memcpy(tag, &addr[0], tag_bits);
tag[tag_bits] = '\0';


int lines = cache_size / block_size;

int i = 0;
for(i = 0; i < lines; i++) {


	if(c->sets[0].lines[i].valid == 1 && strcmp(c->sets[0].lines[i].tag, tag) == 0) {
	/*Cache Hit!!*/
	if(rw == 'W') {
	cache_hits++;
	
	if(wt == 1) {
		mem_writes++;
	} 
	
	x++;
	c->sets[0].lines[i].y = x;
	c->sets[0].lines[i].dirty = 1;
	
	} else
	if(rw == 'R') {
	cache_hits++;
	x++;
	c->sets[0].lines[i].y = x;
		} 
	return;
	}

 }

	/*Cache Miss! */
	
	/*Find A non valid line*/

	Line* ln = NULL;
	int j = 0;
	for(j = 0; j < lines; j++) {
		if(c->sets[0].lines[j].valid == 0) {
		/*Empty Line found */
		ln = &(c->sets[0].lines[j]);
		}
	}
	
	int oldest = REALLY_OLD;
	if(ln == NULL) {
	/*Need to find OLDEST line.*/
	int k = 0;
	for(k = 0; k < lines; k++) {
		if(c->sets[0].lines[k].y < oldest) {
		ln = &(c->sets[0].lines[k]);
		oldest = c->sets[0].lines[k].y;
		
		}
	}
	
	}
	int l = 0;
	for(l = 0; l < strlen(ln->tag); l++) {
			ln->tag[l] = '0';
		}
	
	if(ln == NULL) {
	printf("NULL ERROR!");
	}
	
	if(rw == 'W') {
	cache_misses++;
	mem_reads++;
	
	if(wt == 1) {
	mem_writes++;
	}
	
	if(wt == 0 && ln->dirty == 1) {
	mem_writes++;
	}
	
	ln->dirty = 1;
	
	} else 
	if(rw == 'R') {
	cache_misses++;
	mem_reads++;
	
	if(wt == 0 && ln->dirty == 1) {
	mem_writes++;
	ln->dirty = 0;
	}
	
	}
	
	x++;
	strcpy(ln->tag, tag);
	ln->y = x;
	ln->valid = 1;

}




void writeToDirectCache(cache* c, char* addr) {
/*Calculate Set bits and Tag Bits of the address. */

char tag[tag_bits + 1];
memcpy(tag, &addr[0], tag_bits);
tag[tag_bits] = '\0';

char set[set_bits + 1];
memcpy(set, &addr[tag_bits], set_bits);
set[set_bits] = '\0';


int set_t = strtol(set, NULL, 2);



/*Check to see if a valid entry is in the current line*/




/*Compare tags*/
	if(c->sets[set_t].lines[0].valid == 1 && strcmp(c->sets[set_t].lines[0].tag, tag) == 0) {
	/*Cache hit!*/
	
	if(rw == 'W') {
	cache_hits++;
	
	if(wt == 1) {
		mem_writes++;
	}
	
	c->sets[set_t].lines[0].dirty = 1;
	
	} else
	if(rw == 'R') {
	cache_hits++;
	
	}
	
	
	} else {
	/*Cache Miss*/
	if(rw == 'W') {
	cache_misses++;
	mem_reads++;
	
	if(wt == 1) {
	mem_writes++;
	}
	
	if(wt == 0 && c->sets[set_t].lines[0].dirty == 1) {
	mem_writes++;
	}
	
	c->sets[set_t].lines[0].dirty = 1;
	
	} else 
	if(rw == 'R') {
	cache_misses++;
	mem_reads++;
	
	if(wt == 0 && c->sets[set_t].lines[0].dirty == 1) {
	mem_writes++;
	c->sets[set_t].lines[0].dirty = 0;
	}
	
	}
	
	strcpy(c->sets[set_t].lines[0].tag, tag);
	c->sets[set_t].lines[0].valid = 1;

	}	








}



void writeToNAssoc(cache* c, char* addr) {

char tag[tag_bits + 1];
memcpy(tag, &addr[0], tag_bits);
tag[tag_bits] = '\0';

char set[set_bits + 1];
memcpy(set, &addr[tag_bits], set_bits);
set[set_bits] = '\0';


int set_t = strtol(set, NULL, 2);

int i = 0;
for(i = 0; i < c->n; i++) {

	if(c->sets[set_t].lines[i].valid == 1 && strcmp(c->sets[set_t].lines[i].tag, tag) == 0) {
	/*Cache Hit!!*/
	if(rw == 'W') {
	cache_hits++;
	
	if(wt == 1) {
		mem_writes++;
	} 
	
	x++;
	c->sets[set_t].lines[i].y = x;
	c->sets[set_t].lines[i].dirty = 1;
	
	} else
	if(rw == 'R') {
	cache_hits++;
	x++;
	c->sets[set_t].lines[i].y = x;
		} 
	return;
	}

 }

	/*Cache Miss!*/
	
	/*Find A non valid line*/

	Line* ln = NULL;
	int j = 0;
	for(j = 0; j < c->n; j++) {
		if(c->sets[set_t].lines[j].valid == 0) {
		/*Empty Line found*/
		ln = &(c->sets[set_t].lines[j]);
		}
	}
	
	int oldest = REALLY_OLD;
	if(ln == NULL) {
	/*Need to find OLDEST line.*/
	int k = 0;
	for(k = 0; k < c->n; k++) {
		if(c->sets[set_t].lines[k].y < oldest) {
		ln = &(c->sets[set_t].lines[k]);
		oldest = c->sets[set_t].lines[k].y;
		
		}
	}
	
	}
	int l = 0;
	for(l = 0; l < strlen(ln->tag); l++) {
			ln->tag[l] = '0';
		}
	
	if(ln == NULL) {
	printf("NULL ERROR!");
	}
	
	if(rw == 'W') {
	cache_misses++;
	mem_reads++;
	
	if(wt == 1) {
	mem_writes++;
	}
	
	if(wt == 0 && ln->dirty == 1) {
	mem_writes++;
	}
	
	ln->dirty = 1;
	
	} else 
	if(rw == 'R') {
	cache_misses++;
	mem_reads++;
	
	if(wt == 0 && ln->dirty == 1) {
	mem_writes++;
	ln->dirty = 0;
	}
	
	}
	
	x++;
	strcpy(ln->tag, tag);
	ln->y = x;
	ln->valid = 1;



}

