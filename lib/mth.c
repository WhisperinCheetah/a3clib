#include "a3clib.h"

int cl_abs(int n) {
	return n >= 0 ? n : n * (-1);
}

long cl_labs(long n) {
	return n >= 0 ? n : n * (-1);
}

long long cl_llabs(long long n) {
	return n >= 0 ? n : n * (-1);
}


float cl_fabsf(float x) {
	return x >= 0. ? x : x * (-1.);
}

double cl_fabs(float x) {
	return x >= 0. ? x : x * (-1.);
}

float cl_fabsl(float x) {
	return x >= 0. ? x : x * (-1.);
}
