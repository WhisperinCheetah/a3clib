#include "a3clib.h"

int cl_atoi(const char *str) {
	int res = 0;
	int count = 0;
	while (str[count] != '\0') {
		res *= 10;
		res += ((int)str[count]) - 48;
		count++;
	}

	return res;
}
