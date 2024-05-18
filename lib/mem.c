#include "clib.h"

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
