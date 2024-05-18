#include <stdio.h>
#include <time.h>
#include "clib.h"

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

int main() {
	srand(time(NULL));
	
	static int test_count = 5;
	static int (*tests[]) () = {
		test_merge_sort,
		test_double_bubble_sort,
		test_memcpy,
		test_memcmp,
		test_atoi,
	};
	
	printf("======== STARTING %d TESTS ============\n", test_count);
	int failed = 0;
	int success = 0;
	for (int i = 0; i < test_count; i++) {
		int code = tests[i]();
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
