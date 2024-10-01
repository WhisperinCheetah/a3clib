#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "a3clib.h"

typedef struct _TestNode {
	int value;
	struct _TestNode* next;
} TestNode;

static int COUNT = 100000;

int main() {
	clock_t start;
	clock_t end;
	void* p;
	
	start = clock();
	for (int i = 0; i < COUNT; i++) {
		p = malloc(sizeof(TestNode));
		if (p == NULL) {
			printf("malloc full after %d nodes\n", i);
			break;
		}
	}
	end = clock();
	float malloc_seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Time for malloc: %f\n", malloc_seconds);

	// Make sure heap is initialized
	p = cl_malloc(sizeof(TestNode));
	cl_free(p);
	start = clock();
	for (int i = 0; i < COUNT; i++) {
		p = cl_malloc(sizeof(TestNode));
		if (p == NULL) {
			printf("malloc full after %d nodes\n", i);
			break;
		}
		
	}
	end = clock();
	float cl_seconds = (float)(end - start) / CLOCKS_PER_SEC;

	printf("Time for cl_mal: %f\n", cl_seconds);
	return 0;
}
