#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lib/collections/vector.h"
#include "lib/error.h"
#include "lib/mth.h"

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

int64_t find_closest(int64_t value, vector_i64_t* others) {
	size_t l = 0;
	size_t r = vector_i64_size(others) - 1;

	if (value < vector_i64_get(others, l)) {
		return vector_i64_get(others, l);
	} else if (value > vector_i64_get(others, r)) {
		return vector_i64_get(others, r);
	}

	while (l <= r) {
		size_t mid = (l + r) / 2;
		int64_t midValue = vector_i64_get(others, mid);

		if (value < midValue) {
			if (r == 0) break;
			r = mid - 1;
		} else if (value > midValue) {
			l = mid + 1;
		} else {
			return midValue;
		}
	}

	if (l > vector_i64_size(others) - 1) {
		return vector_i64_get(others, vector_i64_size(others) - 1);
	} else {
		int64_t lValue = vector_i64_get(others, l);
		int64_t rValue = vector_i64_get(others, r);
		return labs(lValue - value) < labs(rValue - value) ? lValue : rValue;
	}
}

int main(void) {
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

	printf("Array A:\n");
	for (int i = 0; i < vector_i64_size(&a); ++i) {
		printf("%lld ", vector_i64_get(&a, i));
	}

	printf("\nArray B:\n");
	for (int i = 0; i < vector_i64_size(&b); ++i) {
		printf("%lld ", vector_i64_get(&b, i));
	}

	if (!vector_i64_sort(&b)) {
		error = error_oops("failed to sort vector B");
		goto cleanup;
	}

	c = vector_i64_create_with_capacity(vector_i64_size(&a));

	for (size_t i = 0; i != vector_i64_size(&a); ++i) {
		int64_t value = vector_i64_get(&a, i);
		int64_t closest = find_closest(value, &b);

		if (!vector_i64_push_back(&c, value + closest)) {
			error = ERROR_HEAP_ALLOCATION;
			goto cleanup;
		}
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
