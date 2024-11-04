#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "task.h"

void print_base(long n, int r) {
	char* result = convert(n, r);

	if (result) printf("%s\n", result);

	free(result);
}

int main(void) {
	print_base(LONG_MIN, 4);
	print_base(LONG_MAX, 4);
	print_base(LONG_MIN, 1);
	print_base(LONG_MAX, 1);
	print_base(0, 2);
	print_base(123456789, 5);
}
