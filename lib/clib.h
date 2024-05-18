#ifndef _CLIB_H
#define _CLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* HEADERS */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define ELEMENT_AT(arr, i, w) ((char*)arr + ((i)*(w)))

/* MEMORY FUNCTIONS */
void cl_memcpy(void *dest, void *src, int count);
int cl_memcmp(const void* r, const void* l, int count);

/* STRING FUNCTIONS */
int cl_atoi(const char *str);


/* ALGORITHMS */
int cl_compare_int(void* v1, void* v2);
int cl_compare_int_reverse(void* v1, void* v2);

void cl_bubble_sort(void* arr, int (*compare)(void*, void*), int size, int count);
void cl_double_bubble_sort(void* arr, int (*compare)(void*, void*), int size, int count, void* palarr, int palsize);
void cl_merge_sort(void* arr, int (*compare)(void*, void*), int size, int count);



#endif
