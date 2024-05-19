#include <stdio.h>
#include <time.h>
#include "clib.h"

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
	const int size = 1000;
	int *arr1 = (int*)malloc(sizeof(int) * size);
	int *arr2 = (int*)malloc(sizeof(int) * size);
	int *arr3 = (int*)malloc(sizeof(int) * size);
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
	return 0;
}

int test_memcmp() {
	const int size = 1000;
	char *arr1 = (char*)malloc(sizeof(char) * size);
	char *arr2 = (char*)malloc(sizeof(char) * size);
	char *arr3 = (char*)calloc(sizeof(char), size);

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

	return 0;
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

	return status;
}

int test_cl_malloc2() {
	void* ptr = cl_malloc(10000);
	if (ptr != NULL) {
		printf("Allocated array bigger than heap size (?\?\?)\n");
		return FAILURE;
	}

	return SUCCESS;
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

typedef int (*test_ptr)();

int main() {
	srand(time(NULL));

	const int test_count = 9;
	const test_ptr tests[] = {
		test_merge_sort,
		test_double_bubble_sort,
		test_memcpy,
		test_memcmp,
		test_atoi,
		test_cl_array,
		test_cl_malloc1,
		test_cl_malloc2,
		test_cl_hashmap,
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
