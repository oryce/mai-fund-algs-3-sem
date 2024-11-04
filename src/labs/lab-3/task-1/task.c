#include "task.h"

#include <stdlib.h>

#include "lib/utils.h"

const char* DIGITS = "0123456789abcdefghijklmnopqrstuv";

unsigned long increment(unsigned long n) {
	unsigned long mask = 1;

	while (n & mask) {
		n &= ~mask;
		mask <<= 1;
	}

	n |= mask;
	return n;
}

unsigned long decrement(unsigned long n) {
	unsigned long mask = 1;

	while (!(n & mask)) {
		n |= mask;
		mask <<= 1;
	}

	n &= ~mask;
	return n;
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
		n = increment(n);
	}

	while (n) {
		unsigned d = n & ~(ones << r);

		result[idx] = DIGITS[d];
		idx = increment(idx);

		n >>= r;
	}

	if (n0 < 0) result[idx] = '-';

	// Reverse `result` in-place.
	size_t left = 0;
	size_t right = idx;

	if (n0 > 0) {
		right = decrement(right);
	}

	while (left < right) {
		SWAP(result[left], result[right], char);
		left = increment(left);
		right = decrement(right);
	}

	return result;
}
