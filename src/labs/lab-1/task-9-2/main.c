#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lib/error.h"
#include "lib/mth.h"
#include "lib/vector.h"

error_t random_vec(vector_i64_t* vec) {
	size_t size = (size_t)mth_rand(10, 10000 + 1);
	*vec = vector_i64_create_with_capacity(size);

	for (int i = 0; i != size; ++i) {
		long value = mth_rand(-1000, 1000);
		if (!vector_i64_push_back(vec, value)) {
			return ERROR_HEAP_ALLOCATION;
		}
	}

	return ERROR_SUCCESS;
}

int main(int argc, char** argv) {
	error_t error;

	srand(time(NULL));  // NOLINT(*-msc51-cpp)

	vector_i64_t a = {.size = -1};
	vector_i64_t b = {.size = -1};
	vector_i64_t c = {.size = -1};

	error = random_vec(&a);
	if (error) {
		fprintf(stderr, "Failed to fill vector A.\n");
		goto cleanup;
	}

	error = random_vec(&b);
	if (error) {
		fprintf(stderr, "Failed to fill vector B.\n");
		goto cleanup;
	}

	c = vector_i64_create_with_capacity(vector_i64_size(&a));

	for (int i = 0; i != vector_i64_size(&a); ++i) {
		long aValue = vector_i64_get(&a, i);

		// Find closest to value
		long difference = -1;
		long closest = 0;

		for (int j = 0; j != vector_i64_size(&b); ++j) {
			long bValue = vector_i64_get(&b, j);
			long newDiff = labs(aValue - bValue);

			if (difference == -1 || newDiff < difference) {
				difference = newDiff;
				closest = bValue;
			}
		}

		if (!vector_i64_push_back(&c, aValue + closest)) {
			error = ERROR_HEAP_ALLOCATION;
			goto cleanup;
		}
	}

	printf("Array A:\n");
	for (int i = 0; i < vector_i64_size(&a); ++i) {
		printf("%lld ", vector_i64_get(&a, i));
	}

	printf("\nArray B:\n");
	for (int i = 0; i < vector_i64_size(&b); ++i) {
		printf("%lld ", vector_i64_get(&b, i));
	}

	printf("\nArray C:\n");
	for (int i = 0; i < vector_i64_size(&c); ++i) {
		printf("%lld ", vector_i64_get(&c, i));
	}

cleanup:
	if (vector_i64_size(&a) != -1) vector_i64_destroy(&a);
	if (vector_i64_size(&b) != -1) vector_i64_destroy(&b);
	if (vector_i64_size(&c) != -1) vector_i64_destroy(&c);

	if (error) {
		error_print(error);
		return (int)-error;
	}
}
