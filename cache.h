#define REALLY_OLD 2147483647;

typedef struct Line Line;
struct Line {

int valid;
int dirty;
char* tag;
int y;

};

typedef struct Set Set;
struct Set {

Line* lines;


};

typedef struct cache cache;
struct cache {

int direct;
int f_assoc;
int n_assoc;
int n;

Set* sets;

};

void printResults(void);
void writeToDirectCache(cache* c, char* addr);
void writeToFullAssoc(cache* c, char* addr);
void writeToNAssoc(cache* c, char* addr);
void writeToCache(cache* c, char* addr);
cache* createCache(int n);
void printResults(void);
char* processHexToBin(char* input);