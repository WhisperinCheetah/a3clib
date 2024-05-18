#include "clib.h"


int cl_compare_int(void* v1, void* v2) {
	return *((int*)v1) - *((int*)v2);
}

int cl_compare_int_reverse(void* v1, void* v2) {
	return *((int*)v2) - *((int*)v1);
}


void cl_bubble_sort(void* arr, int (*compare)(void*, void*), int size, int count) {
	if (!arr || !compare || !size || !count) {
		return;
	}

	void* buff = calloc(size, 1);
	for (int j = 0; j < count; j++) {
		for (int i = 0; i < count - 1; i++) {
			if (compare(ELEMENT_AT(arr, i, size), ELEMENT_AT(arr, i+1, size)) > 0) {
				memcpy(buff, ELEMENT_AT(arr, i, size), size);
				memcpy(ELEMENT_AT(arr, i, size), ELEMENT_AT(arr, i+1, size), size);
				memcpy(ELEMENT_AT(arr, i+1, size), buff, size);
			}
		}
	}
	free(buff);
}

void cl_double_bubble_sort(void* arr, int (*compare)(void*, void*), int size, int count, void* palarr, int palsize) {
	if (!arr || !compare || !size || !count || !palarr || !palsize) {
		return;
	}

	void* buff = calloc(size, 1);
	void* palbuff = calloc(palsize, 1);
	for (int j = 0; j < count; j++) {
		for (int i = 0; i < count - 1; i++) {
			if (compare(ELEMENT_AT(arr, i, size), ELEMENT_AT(arr, i+1, size)) > 0) {
				memcpy(buff, ELEMENT_AT(arr, i, size), size);
				memcpy(ELEMENT_AT(arr, i, size), ELEMENT_AT(arr, i+1, size), size);
				memcpy(ELEMENT_AT(arr, i+1, size), buff, size);

				memcpy(palbuff, ELEMENT_AT(palarr, i, palsize), palsize);
				memcpy(ELEMENT_AT(palarr, i, palsize), ELEMENT_AT(palarr, i+1, palsize), palsize);
				memcpy(ELEMENT_AT(palarr, i+1, palsize), palbuff, palsize);
			}
		}
	}
	free(buff);
}


void merge_sort_helper(void* arr, int lb, int ub, int (*compare)(void*, void*), int size) {
	if (ub - lb <= 1) {
		return;
	}

	int count = ub - lb;
	int center = ((ub - lb) / 2) + lb;

	merge_sort_helper(arr, lb, center, compare, size);
	merge_sort_helper(arr, center, ub, compare, size);

	void* buffer = calloc(size, count);
	if (buffer == NULL) {
		printf("Could not allocate merge_sort_helper buffer\n");
		exit(1);
	}
	int p1 = lb;
	int p2 = center;
	for (int i = 0; i < count; i++) {
		if (p2 >= ub || (p1 < center && compare(ELEMENT_AT(arr, p1, size), ELEMENT_AT(arr, p2, size)) <= 0)) {
			memcpy(ELEMENT_AT(buffer, i, size), ELEMENT_AT(arr, p1, size), size);
			p1++;
		} else {
			memcpy(ELEMENT_AT(buffer, i, size), ELEMENT_AT(arr, p2, size), size);
			p2++;
		}
	}
	memcpy(ELEMENT_AT(arr, lb, size), buffer, size*count);
}

void cl_merge_sort(void* arr, int (*compare)(void*, void*), int size, int count) {
	if (!arr || !compare || !size || !count) {
		return;
	}

	merge_sort_helper(arr, 0, count, compare, size);
}


