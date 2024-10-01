#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "a3clib.h"

typedef struct _TestNode {
	int value;
	struct _TestNode* next;
} TestNode;

static int COUNT = 10000;

int main() {
	clock_t start;
	clock_t end;
	//	void* p;

	TestNode **arr1 = malloc(sizeof(TestNode*) * COUNT);
	
	start = clock();
	for (int i = 0; i < COUNT; i++) {
		arr1[i] = malloc(sizeof(TestNode));
		if (arr1[i] == NULL) {
			printf("malloc full after %d nodes\n", i);
			break;
		}
	}

	for (int i = 0; i < COUNT; i++) {
		free(arr1[i]);
	}

		for (int i = 0; i < COUNT; i++) {
		arr1[i] = malloc(sizeof(TestNode));
		if (arr1[i] == NULL) {
			printf("malloc full after %d nodes\n", i);
			break;
		}
	}

	for (int i = 0; i < COUNT; i++) {
		free(arr1[i]);
	}
	end = clock();
	float malloc_seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Time for malloc: %f\n", malloc_seconds);

	// Make sure heap is initialized
	void* p = cl_malloc(sizeof(TestNode));
	cl_free(p);
	start = clock();
	for (int i = 0; i < COUNT; i++) {
		arr1[i] = cl_malloc(sizeof(TestNode));
		if (arr1[i] == NULL) {
			printf("malloc full after %d nodes\n", i);
			break;
		}
	}

	for (int i = 0; i < COUNT; i++) {
		cl_free(arr1[i]);
	}

		for (int i = 0; i < COUNT; i++) {
		arr1[i] = cl_malloc(sizeof(TestNode));
		if (arr1[i] == NULL) {
			printf("malloc full after %d nodes\n", i);
			break;
		}
	}

	for (int i = 0; i < COUNT; i++) {
		cl_free(arr1[i]);
	}

	end = clock();
	float cl_seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Time for cl_mal: %f\n", cl_seconds);
	/*
	printf("Testing if cl_malloc can still allocate bigger elements at reasonable pace\n");
	printf("Remaining bytes: %d\n", cl_remaining_heap_size());
	p = cl_malloc(cl_remaining_heap_size() - 8);
	if (p == NULL) {
		fprintf(stderr, "Could not allocate %d bytes anymore\n", cl_remaining_heap_size() - 8);
		return 1;
	} else {
		printf("Allocated %ld bytes\n", sizeof(TestNode) * COUNT);
		printf("Remaining bytes: %d\n", cl_remaining_heap_size());
	}
	*/
	return 0;
}
