#include "task.h"

#include <stdlib.h>

const char* DIGITS = "0123456789abcdefghijklmnopqrstuv";

static void increment(unsigned long* n) {
	unsigned long mask = 1;

	while (*n & mask) {
		*n &= ~mask;
		mask <<= 1;
	}

	*n |= mask;
}

static void decrement(unsigned long* n) {
	unsigned long mask = 1;

	while (!(*n & mask)) {
		*n |= mask;
		mask <<= 1;
	}

	*n &= ~mask;
}

char* convert(long n0, int r) {
	char* result = (char*)calloc(66, sizeof(char));
	if (!result) return NULL;

	unsigned long ones = ~0;
	size_t idx = 0;

	if (n0 == 0) {
		result[idx] = '0';
		return result;
	}

	unsigned long n = n0;

	if (n0 < 0) {
		// Convert from two's complement.
		n = ~n;
		increment(&n);
	}

	while (n) {
		unsigned d = n & ~(ones << r);

		result[idx] = DIGITS[d];
		increment(&idx);

		n >>= r;
	}

	if (n0 < 0) result[idx] = '-';

	// Reverse `result` in-place.
	size_t left = 0;
	size_t right = idx;

	if (n0 > 0) decrement(&right);

	while (left < right) {
		char temp = result[left];
		result[left] = result[right];
		result[right] = temp;

		increment(&left);
		decrement(&right);
	}

	return result;
}
