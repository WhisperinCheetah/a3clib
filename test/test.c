#include <stdio.h>
#include <time.h>
#include "a3clib.h"

#include <stdbool.h>
#include <sys/mman.h>
#include <unistd.h>

int assert(int expected, int actual, const char* message) {
	if (expected != actual) {
		printf("ASSERTION FAILED! expected %d, got %d\n", expected, actual);
		printf("%s", message);
		return FAILURE;
	}

	return SUCCESS;
}

void assert_st(STATUS* status, int expected, int actual, const char* message) {
	STATUS res = assert(expected, actual, message);
	if (res != SUCCESS) {
		*status = res;
	}
}

void assertf_st(STATUS* status, float expected, float actual, const char* message) {
	STATUS res = assert(expected, actual, message);
	if (res != SUCCESS) {
		*status = res;
	}
}


STATUS assert_ptr(void* expected, void* actual, const char* message) {
	if (expected != actual) {
		printf("ASSERTION FAILED! expected %p, got %p\n", expected, actual);
		printf("%s", message);
		return FAILURE;
	}
	return SUCCESS;
}

STATUS assert_ptr_ne(void* unexpected, void* actual, const char* message) {
	if (unexpected == actual) {
		printf("ASSERTION FAILED! expected %p, got %p\n", unexpected, actual);
		printf("%s", message);
		return FAILURE;
	}
	return SUCCESS;
}


void assert_ptr_st(STATUS* status, void* expected, void* actual, const char* message) {
	STATUS res = assert_ptr(expected, actual, message);

	if (res != SUCCESS) {
		*status = res;
	}
}

void assert_ptr_ne_st(STATUS* status, void* expected, void* actual, const char* message) {
	STATUS res = SUCCESS;
	if (expected == actual) {
		printf("ASSERTION FAILED! expected NOT %p, got %p\n", expected, actual);
		printf("%s", message);
		res = FAILURE;
	}

	if (res != SUCCESS) {
		*status = res;
	}
}

int test_merge_sort() {
	cl_memcpy(NULL, NULL, 0);


	const int size1 = 2000000;
	int *arr1 = (int*)malloc(sizeof(int) * size1);
	for (int i = 0; i < size1; i++) {
		arr1[i] = rand() % size1*5;
	}

	printf("Created random array\n");
	
	clock_t start = clock();
	cl_merge_sort(arr1, cl_compare_int, sizeof(int), size1);
	clock_t end = clock();

	for (int i = 0; i < size1 - 1; i++) {
		if (arr1[i] > arr1[i+1]) {
			printf("Error! Found elements not in order\n");
			printf("Elements: ");
			for (int j = MAX(i-2, 0); j <= MIN(i+2, size1-1); j++) {
				printf("%d ", arr1[j]);
			}
			printf("\n");
			return 1;
		}
	}

	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("Completed merge sort of %d elements in %f seconds\n", size1, seconds);

	free(arr1);

	return 0;
}

int test_double_bubble_sort() {
	return 0;
}

int test_memcpy() {
	const int size = 250;
	int *arr1 = (int*)cl_malloc(sizeof(int) * size);
	int *arr2 = (int*)cl_malloc(sizeof(int) * size);
	int *arr3 = (int*)cl_malloc(sizeof(int) * size);
	printf("test_cl_memcpy(): received pointers\n");
	printf("\t%p\n", arr1);
	printf("\t%p\n", arr2);
	printf("\t%p\n", arr3);

	for (int i = 0; i < size; i++) {
		arr1[i] = rand() % size;
	}

	cl_memcpy(arr2, arr1, sizeof(int) * size);
	memcpy(arr2, arr1, sizeof(int) * size);

	for (int i = 0; i < size; i++) {
		if (arr2[i] != arr1[i]) {
			printf("test_memcpy() failed! elements in pos %d differ\n", i);
			printf("relevent elements include: \n");
			printf("arr2[i] == %d (cl_memcpy)\n", arr2[i]);
			printf("arr3[i] == %d (memcpy)\n", arr3[i]);
			return 1;
		}
	}

	cl_free(arr3);
	cl_free(arr2);
	cl_free(arr1);
	
	return 0;
}

int test_memcmp() {
	const int size = 1000;
	printf("Wanted size: %ld\n", sizeof(char) * size);
	char *arr1 = (char*)cl_malloc(sizeof(char) * size);
	char *arr2 = (char*)cl_malloc(sizeof(char) * size);
	char *arr3 = (char*)cl_calloc(sizeof(char), size);

	if (arr1 == NULL || arr2 == NULL || arr3 == NULL) {
		fprintf(stderr, "Could not allocate enough memory for arrays\n");
		fprintf(stderr, "Relevant pointers:\n");
		fprintf(stderr, "char *arr1 = %p\n", arr1);
		fprintf(stderr, "char *arr2 = %p\n", arr2);
		fprintf(stderr, "char *arr3 = %p\n", arr3);
		return FAILURE;
	}

	printf("char *arr1 = %p\n", arr1);
	printf("char *arr2 = %p\n", arr2);
	printf("char *arr3 = %p\n", arr3);
	
	for (int i = 0; i < 1000; i++) {
		arr1[i] = 'a';
		arr2[i] = 'a';
		arr3[i] = 0;
	}

	int v = cl_memcmp(arr1, arr2, size);
	if (cl_memcmp(arr1, arr2, size) != 0) {
		printf("test_memcmp() failed!\n");
		printf("comparison between arr1 and arr2 should return 0, but returned %d\n", v);
		return 1;
	}

	v = cl_memcmp(arr1, arr3, size);
	if (v <= 0) {
		printf("test_memcmp() failed!\n");
		printf("comparison between arr1 and arr3 should return a value >0, but returned %d\n", v);
		return 1;
	}


	v = cl_memcmp(arr3, arr1, size);
	if (v >= 0) {
		printf("test_memcmp() failed!\n");
		printf("comparison between arr3 and arr1 should return a value <0, but did not\n");
		return 1;
	}

	printf("Remaining after test 4: %d\n", cl_remaining_heap_size());
	
	cl_free(arr3);
	cl_free(arr2);
	cl_free(arr1);

	printf("Remaining after test 4: %d\n", cl_remaining_heap_size());
	
	return 0;
}

int test_memset() {
	int size = getpagesize() / 2;
	char* ptr = (char*)cl_malloc(size);
	cl_memset(ptr, 'c', size);

	STATUS st = SUCCESS;
	for (int i = 0; i < size; i++) {
		assert_st(&st, 'c', ptr[i], "memset did not fill buffer with 'c'\n");
	}

	cl_free(ptr);

	return st;
}

int test_atoi() {
	const char* str1 = "12";
	int v1 = cl_atoi(str1);
	int v1actual = atoi(str1);
	if (v1 != v1actual) {
		printf("test_atoi() failed!\n");
		printf("v1 should be %d but was %d\n", v1actual, v1);
		return 1;
	}

	const char* str2 = "0092847";
	int v2 = cl_atoi(str2);
	int v2actual = atoi(str2);
	if (v2 != v2actual) {
		printf("test_atoi() failed!\n");
		printf("v2 should be %d but was %d\n", v2actual, v2);
		return 1;
	}

	return 0;
}

int test_cl_array() {
	int values[] = { 1, 2, 3 };
	cl_array *cl_arr = cl_array_create(sizeof(int));

	for (int i = 0; i < 3; i++) {
		cl_array_add(cl_arr, &(values[i]));
	}

	const char* equal_message = "Dynamic array and normal array do not match!";
	int v;
	for (int i = 0; i < 3; i++) {
		cl_array_get(cl_arr, i, &v);
		if (assert(values[i], v, equal_message) != SUCCESS) {
			return FAILURE;
		}
	}

	const char* count_message = "Array size is not equal to 3\n";
	if (assert(3, cl_arr->count, count_message)) return FAILURE;

	return SUCCESS;
		
}

int test_cl_malloc1() {
	const int size = 4;
	int values1[] = { 1, 2, 3, 4 };
	int values2[] = { 4, 5, 6, 7 };
	int values3[] = { 8, 9, 10, 11 };

	int* ptr1 = (int*)cl_malloc(sizeof(int) * size);
	int* ptr2 = (int*)cl_malloc(sizeof(int) * size);
	int* ptr3 = (int*)cl_malloc(sizeof(int) * size);

	printf("test_cl_malloc1(): received pointers\n");
	printf("\t%p\n", ptr1);
	printf("\t%p\n", ptr2);
	printf("\t%p\n", ptr3);
	
	for (int i = 0; i < size; i++) {
		ptr1[i] = values1[i];
		ptr2[i] = values2[i];
		ptr3[i] = values3[i];
	}

	STATUS status = SUCCESS;
	for (int i = 0; i < size; i++) {
		assert_st(&status, values1[i], ptr1[i], "First heap array and stack array do not match!\n");
		assert_st(&status, values2[i], ptr2[i], "Second heap array and stack array do not match!\n");
		assert_st(&status, values3[i], ptr3[i], "Third heap array and stack array do not match!\n");
	}

	cl_free(ptr3);
	cl_free(ptr2);
	cl_free(ptr1);

	int ptr_size = getpagesize() - 32;
	void* big_ptr = cl_malloc(ptr_size);
	void* sml_ptr = cl_malloc(16);
	assert_ptr_ne_st(&status, NULL, big_ptr, "Pointer should be allocated!\n");
	assert_ptr_ne_st(&status, NULL, sml_ptr, "Pointer should be allocated!\n");

	cl_free(big_ptr);
	cl_free(sml_ptr);
	
	return status;
}

int test_cl_malloc2() {
	int amount = cl_remaining_heap_size();
	printf("Remaining amount: %d\n", amount);

	void* p1 = cl_malloc(amount - 10000);
	void* p2 = cl_malloc(15000);

	STATUS status = SUCCESS;
	assert_ptr_ne_st(&status, NULL, p1, "Could not allocate remaining data - 10000\n");
	assert_ptr_st(&status, NULL, p2, "Allocated beyond the maximum amount of data remaining\n");

	cl_free(p1);
	cl_free(p2);

	return status;
}

int test_cl_calloc() {
	STATUS st = SUCCESS;

	int size = getpagesize() - 16;
	char* ptr = (char*)calloc(size, 1); // Get entire heap
	assert_ptr_ne_st(&st, NULL, ptr, "Could not allocate enough memory\n");
	
	for (int i = 0; i < size; i++) {
		assert_st(&st, 0, ptr[i], "Byte was not set to 0 in calloc call\n");
	}

	return st;
}


int test_cl_hashmap() {
	char* m1 = "woord1\0";
	char* m2 = "woord2\0";
	char* m3 = "woord3\0";

	cl_hashmap map;
	cl_hashmap_init(&map, -1);

	cl_hashmap_add(&map, m1, 10);
	cl_hashmap_add(&map, m2, 20);
	cl_hashmap_add(&map, m3, 30);

	int* v1 = cl_hashmap_get(&map, m1);
	(*v1)++;

	STATUS status = SUCCESS;
	assert_st(&status, 11, *cl_hashmap_get(&map, m1), "First hashmap value is incorrect\n");
	assert_st(&status, 20, *cl_hashmap_get(&map, m2), "Second hashmap value is incorrect\n");
	assert_st(&status, 30, *cl_hashmap_get(&map, m3), "Third hashmap value is incorrect\n");

	cl_hashmap_destroy(&map);

	return status;
}

int test_abs() {
	int x = -3;
	int y = 3;

	int ax = cl_abs(x);
	int ay = cl_abs(y);
	
	int expected = 3;

	STATUS st = SUCCESS;
	assert_st(&st, expected, ax, "abs did not return correct value\n");
	assert_st(&st, expected, ay, "abs did not return correct value\n");
		
	return st;
}

int test_fabsf() {
	float x = -3.;
	float y = 3.;

	float ax = cl_fabsf(x);
	float ay = cl_fabsf(y);

	float exp = 3.;

	STATUS st = SUCCESS;
	assertf_st(&st, exp, ax, "fabsf did not return correct value\n");
	assertf_st(&st, exp, ay, "fabsf did not return correct value\n");
	return st;
}

typedef int (*test_ptr)();

int main() {
	srand(time(NULL));

	const int test_count = 12;
	const test_ptr tests[] = {
		test_merge_sort,
		test_double_bubble_sort,
		test_memcpy,
		test_memcmp,
		test_memset,
		test_atoi,
		test_cl_array,
		test_cl_malloc1,
		test_cl_malloc2,
		test_cl_calloc,
		test_cl_hashmap,
		test_abs,
		test_fabsf,
	};

	printf("======== STARTING %d TESTS ============\n", test_count);
	int failed = 0;
	int success = 0;
	for (int i = 0; i < test_count; i++) {
		int code = (*tests[i])();
		if (code == 0) {
			success++;
		} else {
			printf("TESTING: Failed test %d\n", i);
			failed++;
		}
		printf("TESTING: +%d, -%d\n", success, failed);
	}
	return failed;
}
