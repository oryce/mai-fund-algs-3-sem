#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lib/convert.h"
#include "lib/error.h"

void array_min_max(long array[], size_t nItems, long a, long b) {
	if (nItems == 0) {
		fprintf(stderr, "Error: no items supplied.\n");
		return;
	}

	long* min = NULL;
	long* max = NULL;

	for (size_t i = 0; i != nItems; ++i) {
		long value = rand() % (b + 1 - a) + a;  // NOLINT(*-msc50-cpp)
		array[i] = value;

		if (min == NULL || value <= *min) {
			min = array + i;
		}
		if (max == NULL || value >= *max) {
			max = array + i;
		}

		printf("[%zu] %ld\n", i, value);
	}

	if (min == NULL || max == NULL) {
		fprintf(stderr, "Catastrophic failure: no min/max found.\n");
		return;
	}

	printf("min: %ld, max: %ld\n", *min, *max);

	long temp = *min;
	*min = *max;
	*max = temp;

	for (size_t i = 0; i != nItems; ++i) {
		printf("[%zu] %ld\n", i, array[i]);
	}
}

int main(int argc, char** argv) {
	srand(time(NULL));  // NOLINT(*-msc51-cpp)

	error_t error;

	if (argc != 3) {
		fprintf(stderr,
		        "Usage: %s <a> <b>\n"
		        "Finds the min and max values in an array of random elements in [a; b]\n",
		        argv[0]);
		return -ERROR_INVALID_PARAMETER;
	}

	long a, b;

	error = str_to_long(argv[1], &a);
	if (error) {
		fprintf(stderr, "Invalid 'a': malformed number or out of range.\n");
		return -ERROR_INVALID_PARAMETER;
	}

	error = str_to_long(argv[2], &b);
	if (error) {
		fprintf(stderr, "Invalid 'b': malformed number or out of range.\n");
		return -ERROR_INVALID_PARAMETER;
	}

	long array[100];
	array_min_max(array, 100, a, b);
}
