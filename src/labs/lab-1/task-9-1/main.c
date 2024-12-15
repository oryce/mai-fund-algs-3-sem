#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lib/convert.h"
#include "lib/error.h"

error_t task(long array[], size_t nItems, long a, long b) {
	if (nItems == 0) return ERROR_INVALID_PARAMETER;

	long* min = NULL;
	long* max = NULL;

	for (size_t i = 0; i != nItems; ++i) {
		long value = rand() % (b + 1 - a) + a;  // NOLINT(*-msc50-cpp)
		array[i] = value;

		if (!min || value <= *min) {
			min = array + i;
		}
		if (!max || value >= *max) {
			max = array + i;
		}

		printf("[%zu] %ld\n", i, value);
	}

	ASSERT(!min || !max);
	printf("min: %ld, max: %ld\n", *min, *max);

	long temp = *min;
	*min = *max;
	*max = temp;

	for (size_t i = 0; i != nItems; ++i) {
		printf("[%zu] %ld\n", i, array[i]);
	}

	return 0;
}

error_t main_(int argc, char** argv) {
	srand(time(NULL));  // NOLINT(*-msc51-cpp)

	if (argc != 3) {
		fprintf(stderr,
		        "Usage: %s <a> <b>\n"
		        "Finds the min and max values in an array of random elements in [a; b]\n",
		        argv[0]);
		return 0;
	}

	long a, b;
	if (str_to_long(argv[1], &a)) {
		fprintf(stderr, "Invalid arguments: malformed 'a'\n");
		return 0;
	}
	if (str_to_long(argv[2], &b)) {
		fprintf(stderr, "Invalid arguments: malformed 'b'\n");
		return 0;
	}

	long array[100];
	return task(array, 100, a, b);
}

int main(int argc, char** argv) {
	error_t error = main_(argc, argv);
	if (error) {
		error_print(error);
		return error;
	}
}
