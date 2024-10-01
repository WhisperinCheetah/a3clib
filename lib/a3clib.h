#ifndef _CLIB_H
#define _CLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* HEADERS */
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define ELEMENT_AT(arr, i, w) ((char*)arr + ((i)*(w)))

#define CL_ARRAY_SIZE 100
#define CL_HASHMAP_SIZE 256

/* STRUCTS */
typedef enum _STATUS {
	SUCCESS=0,
	FAILURE,
	MEMORY,
} STATUS;


typedef struct _cl_arr {
	void *data;
	int count;
	int size;
	int element_size;
} cl_array;


typedef struct _cl_kv_pair {
	char* key;
	int val;
	struct _cl_kv_pair* next;
} cl_kv_pair;

typedef struct _cl_hashmap {
	cl_kv_pair** pairs;
	int size;
	int count;
} cl_hashmap;

/* MATH FUNCTIONS */
int cl_abs(int n);
long cl_labs(long n);
long long cl_llabs(long long n);

float cl_fabsf(float x);

/* MEMORY FUNCTIONS */
void cl_memcpy(void *dest, void *src, int count);
int cl_memcmp(const void* r, const void* l, int count);
void cl_memset(void* dest, char c, int count);

void* cl_malloc(int amount);
void* cl_calloc(int size, int count);
void* cl_realloc(void* ptr, int resize);
void cl_free(void* ptr);

/* STRING FUNCTIONS */
int cl_atoi(const char *str);


/* ALGORITHMS */
int cl_compare_int(void* v1, void* v2);
int cl_compare_int_reverse(void* v1, void* v2);

void cl_bubble_sort(void* arr, int (*compare)(void*, void*), int size, int count);
void cl_double_bubble_sort(void* arr, int (*compare)(void*, void*), int size, int count, void* palarr, int palsize);
void cl_merge_sort(void* arr, int (*compare)(void*, void*), int size, int count);

/* DATASTRUCTURES */
STATUS cl_array_init_size(cl_array *cl_arr, int element_size, int size);
STATUS cl_array_init(cl_array *arr, int element_size);
cl_array* cl_array_create(int element_size);
void cl_array_get(cl_array *cl_arr, int index, void *dest);
STATUS cl_array_add(cl_array *cl_arr, void* element);
STATUS cl_array_add_all(cl_array *cl_arr, void* elements, int count);
STATUS cl_array_remove(cl_array *cl_arr, int index);
STATUS cl_array_destroy(cl_array *cl_arr);

void cl_hashmap_init(cl_hashmap* map, int size);
int* cl_hashmap_get(cl_hashmap* map, char* v);
void cl_hashmap_add(cl_hashmap* map, char* k, int v);
void cl_hashmap_destroy(cl_hashmap* map);

#endif
