#include "a3clib.h"

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

void cl_memset(void* dest, char c, int count) {
	if (dest != NULL && count > 0) {
		char* char_dest = (char*)dest;
		for (int i = 0; i < count; i++) {
			char_dest[i] = c;
		}
	}
}


typedef struct _cl_heap_chunk {
	int size;
	bool inuse;
	struct _cl_heap_chunk* next;
	struct _cl_heap_chunk* ll_next;
} cl_heap_chunk;

typedef struct _cl_heap_info {
	cl_heap_chunk* start;
	cl_heap_chunk* first_free;
	int avail;
} cl_heap_info;

static cl_heap_info heap;
static bool initialized = false;
static int heap_size;
cl_heap_chunk* free_list;

STATUS cl_heap_init(cl_heap_info* heap_info) {
	// Standard heap space is 4096 bytes
	// Slight overkill so I chose 1MB
	// 4096 bytes * 256 = 1024*1024bytes = 1024KB = 1MB
	heap_size = getpagesize() * 1024; 
	void* start = mmap(NULL, heap_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (start == (void*)-1) {
		fprintf(stderr, "Failed to initialize heap\n");
		return MEMORY;
	} 

	cl_heap_chunk* first = (cl_heap_chunk*)start; // Put metadata at start
	first->size = heap_size - sizeof(cl_heap_chunk);
	first->inuse = false;
	first->next = NULL;
	first->ll_next = NULL;

	heap_info->start = first;
	heap_info->first_free = first;
	heap_info->avail = first->size;

	free_list = first;

	fprintf(stdout, "Initialized heap @ %p\n", start);
	fprintf(stdout, "Heap size = %d\n", heap_size);
	fprintf(stdout, "sizeof(cl_heap_chunk) = %ld\n", sizeof(cl_heap_chunk));
	fprintf(stdout, "sizeof(cl_heap_info) = %ld\n", sizeof(cl_heap_chunk));
	
	return SUCCESS;
}

void cl_cleanup_heap() {
	// Cleanup on exit
	munmap(heap.start, heap_size);
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
	//printf("Start chunk: %p\n", chunk);
	while (chunk->next != NULL) {
		if (chunk->inuse == false) {
			cl_chunk_defrag(heap_info, chunk);
		}
		chunk = chunk->next;
	}
}

cl_heap_chunk* cl_heap_find_chunk(cl_heap_chunk* start, int amount) {
	cl_heap_chunk* chunk = free_list;
	if (free_list != NULL && free_list->size >= amount) {
		free_list = free_list->ll_next;
		chunk->ll_next = NULL;
		return chunk;
	}

	chunk = free_list;
	while (chunk->ll_next != NULL && chunk->ll_next->size < amount) {
		chunk = chunk->ll_next;
	}

	if (chunk->ll_next != NULL) {
		cl_heap_chunk* chosen = chunk->ll_next;
		chunk->ll_next = chunk->ll_next->ll_next;
		return chosen;
	}

	return NULL;
	// Never gets to here
	chunk = start;
	while ((chunk->inuse || chunk->size < amount) && chunk != NULL) {
		chunk = chunk->next;
	}
	return chunk;
}

void* cl_malloc(int amount) {
	ensure_initialized();
	int aligned_amount = get_aligned(amount, sizeof(cl_heap_chunk)); // Add 16 for metadata about chunk

	if (aligned_amount > heap.avail) {
		// If not enough space, try defragmentation
		// cl_heap_defrag(&heap);
		if (aligned_amount > heap.avail) {
			fprintf(stderr, "Heap space full! available: %d, wanted: %d\n", heap.avail, aligned_amount);
			return NULL;
		}
	} 

	// Go over all chunks and find first one that has size great enough
	// If none are found chunk will be NULL
	cl_heap_chunk* chunk = cl_heap_find_chunk(heap.start, aligned_amount);
	if (chunk == NULL) {
		// Try again after defragging heap
		// cl_heap_defrag(&heap);
		return NULL;
			//chunk = cl_heap_find_chunk(heap.start, aligned_amount);
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
		next->ll_next = free_list;
		free_list = next;

		chunk->next = next;
		chunk->size = aligned_amount;
		chunk->inuse = true;
		
		heap.avail -= 16;
	}

	// printf("Allocated pointer; size=%d, actual size=%d @ %p\n", aligned_amount, chunk->size, chunk+1);
	
	heap.avail -= chunk->size;
	return (void*)(chunk + 1);
}

void* cl_calloc(int size, int count) {
	void* ptr = cl_malloc(size * count);
	if (ptr != NULL) {
		char* byte_ptr = (char*)ptr;
		for (int i = 0; i < size*count; i++) {
			byte_ptr[i] = 0;
		}
	}
	return ptr;
}

void cl_free(void* ptr) {
	ensure_initialized();

	if (ptr == NULL) {
		fprintf(stderr, "Warning: tried to call free on a null-pointer!\n");
		// add check to see if pointer is within heap bounds
		return;
	}
	
	cl_heap_chunk* chunk = ((cl_heap_chunk*)ptr) - 1;
	chunk->inuse = false;
	chunk->ll_next = free_list;
	free_list = chunk;
	// cl_chunk_defrag(&heap, chunk);
	heap.avail += chunk->size;
}

void* cl_realloc(void* ptr, int resize) {
	printf("Realloc not yet implemented! cl_realloc(%p, %d)\n", ptr, resize);
	return NULL;
}

int cl_remaining_heap_size() {
	return heap.avail;
}

