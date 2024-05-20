#include "clib.h"

#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>

void cl_memcpy(void *dest, void *src, int count) {
	char *to = (char*)dest;
	char *from = (char*)src;
	for (int i = 0; i < count; i++) {
		*to = *from;
	}
}

int cl_memcmp(const void* l, const void* r, int count) {
	char* left = (char*)l;
	char* right = (char*)r;
	for (int i = 0; i < count; i++) {
		// Comparing like this halves the amount of comparisons per iteration
		if (left[i] != right[i]) { 
			if (left[i] < right[i]) {
				return -1;
			} else {
				return 1;
			}
		}
	}

	return 0;
}


typedef struct _cl_heap_chunk {
	int size;
	bool inuse;
	struct _cl_heap_chunk* next;
} cl_heap_chunk;

typedef struct _cl_heap_info {
	cl_heap_chunk* start;
	int avail;
} cl_heap_info;

static cl_heap_info heap;
static bool initialized = false;

STATUS cl_heap_init(cl_heap_info* heap_info) {
	void* start = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (start == (void*)-1) {
		printf("Failed to initialize heap\n");
		return MEMORY;
	} 

	cl_heap_chunk* first = (cl_heap_chunk*)start; // Put metadata at start
	first->size = getpagesize() - sizeof(cl_heap_chunk);
	first->inuse = false;
	first->next = NULL;

	heap_info->start = first;
	heap_info->avail = first->size;

	printf("Initialized heap @ %p\n", start);
	printf("sizeof(cl_heap_chunk) = %ld\n", sizeof(cl_heap_chunk));
	printf("sizeof(cl_heap_info) = %ld\n", sizeof(cl_heap_chunk));
	
	return SUCCESS;
}

void cl_cleanup_heap() {
	// Cleanup on exit
}

int get_aligned(int amount, int alignment) {
	// Amount needs to be a multiple of 16 (on 64-bit architectures)
	// This function is also a bit of a mystery for me
	return (amount + alignment - 1) & ~(alignment - 1);
}

void ensure_initialized() {
	if (!initialized) {
		cl_heap_init(&heap);
		initialized = true;
		atexit(cl_cleanup_heap);
	}
}

void cl_chunk_defrag(cl_heap_info* heap_info, cl_heap_chunk* chunk) {
	while (chunk->next != NULL && chunk->next->inuse == false) {
		// Merge forward
		cl_heap_chunk* next = chunk->next;
		chunk->size += next->size + 16; // add 16 since metadata gets removed
		heap_info->avail += 16;
		chunk->next = next->next;
	}
}

void cl_heap_defrag(cl_heap_info* heap_info) {
	cl_heap_chunk* chunk = heap_info->start;
	printf("Start chunk: %p\n", chunk);
	while (chunk->next != NULL) {
		if (chunk->inuse == false) {
			cl_chunk_defrag(heap_info, chunk);
		}
		chunk = chunk->next;
	}
}

cl_heap_chunk* cl_heap_find_chunk(cl_heap_chunk* start, int amount) {
	cl_heap_chunk* chunk = start;
	while ((chunk->inuse || chunk->size < amount) && chunk != NULL) {
		chunk = chunk->next;
	}
	return chunk;
}

void* cl_malloc(int amount) {
	ensure_initialized();
	int aligned_amount = get_aligned(amount, 16); // Add 16 for metadata about chunk

	if (aligned_amount > heap.avail) {
		// If not enough space, try defragmentation
		cl_heap_defrag(&heap);
		if (aligned_amount > heap.avail) {
			printf("Heap space full! available: %d, wanted: %d\n", heap.avail, aligned_amount);
			return NULL;
		}
	} 

	// Go over all chunks and find first one that has size great enough
	// If none are fonud chunk will be NULL
	cl_heap_chunk* chunk = cl_heap_find_chunk(heap.start, aligned_amount);
	if (chunk == NULL) {
		// Try again after defragging heap
		cl_heap_defrag(&heap);
		chunk = cl_heap_find_chunk(heap.start, aligned_amount);
	} 


	// If chunk is greater than the wanted amount and has at least
	// 32 extra remaining, split it.
	// 32 for metadata + at least 16 for other chunk
	if (chunk->size - aligned_amount >= 32) { 
		// Split chunk
		// Get position for next chunk metadata
		cl_heap_chunk* next = chunk + (aligned_amount / 16) + 1; // metadata gets put behind
		next->size = chunk->size - aligned_amount - 16;
		next->inuse = false;
		next->next = chunk->next;

		chunk->next = next;
		chunk->size = aligned_amount;
		chunk->inuse = true;
		heap.avail -= 16;
	}

	printf("Allocated pointer; size=%d, actual size=%d @ %p\n", aligned_amount, chunk->size, chunk+1);
	
	heap.avail -= chunk->size;
	return (void*)(chunk + 1);
}

void cl_free(void* ptr) {
	printf("Freeing %p\n", ptr);
	ensure_initialized();

	cl_heap_chunk* chunk = ((cl_heap_chunk*)ptr) - 1;
	chunk->inuse = false;
	heap.avail += chunk->size;
	cl_chunk_defrag(&heap, chunk);
}

void* cl_calloc(int size, int count) {
	printf("Calloc not yet implemented! cl_calloc(%d, %d)\n", size, count);
	return NULL;
}

void* cl_realloc(void* ptr, int resize) {
	printf("Realloc not yet implemented! cl_realloc(%p, %d)\n", ptr, resize);
	return NULL;
}
