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

int cl_memcmp(const void* r, const void* l, int count) {
	for (int i = 0; i < count; i++) {
		// Comparing like this halves the amount of comparisons per iteration
		if (((char*)l)[i] != ((char*)r)[i]) { 
			if (((char*)l)[i] < ((char*)r)[i]) {
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

void* cl_malloc(int amount) {
	ensure_initialized();
	int aligned_amount = get_aligned(amount, 16) + 16; // Add 16 for metadata about chunk

	if (aligned_amount > heap.avail) {
		return NULL;
	}

	cl_heap_chunk* chunk = heap.start;
	while ((chunk->inuse || chunk->size < amount) && chunk != NULL) {
		chunk = chunk->next;
	}

	if (chunk == NULL) {
		// Try defrag, else return NULL
		return NULL;
	} 

	
	if (chunk->size - aligned_amount >= 16) {
		// Split chunk
		// Get position for next chunk metadata
		cl_heap_chunk* next = chunk + (aligned_amount / 16) - 1;
		next->size = chunk->size - aligned_amount;
		next->inuse = false;
		next->next = chunk->next;

		chunk->next = next;
		chunk->size = aligned_amount;
		chunk->inuse = true;
		heap.avail -= 16;
	}

	heap.avail -= chunk->size;
	return (void*)(chunk + 1);
}

void* cl_calloc(int size, int count) {
	printf("Calloc not yet implemented! cl_calloc(%d, %d)\n", size, count);
	return NULL;
}

void* cl_realloc(void* ptr, int resize) {
	printf("Realloc not yet implemented! cl_realloc(%p, %d)\n", ptr, resize);
	return NULL;
}

void cl_free(void* ptr) {
	ensure_initialized();
	printf("Free not yet implemented! free(%p)\n", ptr);
	// do nothing yet
}
