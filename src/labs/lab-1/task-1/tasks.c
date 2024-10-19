#include "tasks.h"

#include <stdio.h>

#include "lib/mth.h"

error_t print_divisible(long x) {
	if (x <= 0) THROW(IllegalArgumentException, "`x` must be a natural number");

	if (x > 100) {
		fprintf(stdout, "No numbers in [1; 100] are divisible by %ld.\n", x);
		return NO_EXCEPTION;
	}

	for (long i = x; i <= 100; i += x) {
		fprintf(stdout, "%ld is divisible by %ld\n", i, x);
	}

	return NO_EXCEPTION;
}

error_t is_prime_(long x, bool* result) {
	if (x <= 0) THROW(IllegalArgumentException, "`x` must be a natural number");

	if (x == 2 || x == 3) {
		*result = true;
		return NO_EXCEPTION;
	} else if (x == 1 || x % 2 == 0 || x % 3 == 0) {
		*result = false;
		return NO_EXCEPTION;
	}

	for (long i = 5; i * i <= x; i += 6) {
		if (x % i == 0 || x % (i + 2) == 0) {
			*result = false;
			return NO_EXCEPTION;
		}
	}

	*result = true;
	return NO_EXCEPTION;
}

error_t print_is_prime(long x) {
	error_t status;
	bool result;

	if (FAILED((status = is_prime_(x, &result)))) {
		fprintf(stdout, "%s", status.message);
		return NO_EXCEPTION;
	}

	fprintf(stdout, "%ld is a %s number", x, result ? "prime" : "composite");
	return NO_EXCEPTION;
}

error_t print_hex(long x) {
	if (x < 0) x = -x;

	char digits[16];  // log_16(LONG_MAX): max length of number
	int length = 0;

	while (x != 0) {
		long r = x % 16;
		x /= 16;

		char digit = (char)(r < 10 ? '0' + r : 'a' + (r - 10));
		digits[length++] = digit;
	}

	for (int i = length - 1; i >= 0; --i) {
		fprintf(stdout, "%c ", digits[i]);
	}

	fprintf(stdout, "\n");
	return NO_EXCEPTION;
}

error_t print_powers(long x) {
	if (x < 1 || x > 10) {
		fprintf(stderr, "Invalid `x`: must be in [1; 10].\n");
		return NO_EXCEPTION;
	}

	fprintf(stdout, "Base  | Power | Base^Power\n");

	for (int base = 1; base != 10 + 1; ++base) {
		long result = 1;
		for (int power = 1; power != x + 1; ++power) {
			result *= base;
			fprintf(stdout, "%5d | %5d | %10ld\n", base, power, result);
		}
	}

	return NO_EXCEPTION;
}

// sqrt(LONG_MAX) * sqrt(2)
const long SUM_LIMIT = 4294967296;

error_t arithmetic_progression_(long n, long* result) {
	if (n < 1) THROW(IllegalArgumentException, "`n` must be a natural number");
	// NB: simplified to sqrt((1 + x) * x) = x
	if (n >= SUM_LIMIT) THROW(OverflowException, "`n` is too large");

	*result = (long)((long double)(1 + n) / 2 * (long double)n);
	return NO_EXCEPTION;
}

error_t print_sums(long x) {
	error_t error;
	long result;

	if (FAILED((error = arithmetic_progression_(x, &result)))) {
		if (error.code == IllegalArgumentException) {
			fprintf(stderr, "Invalid input: %s\n", error.message);
		} else if (error.code == OverflowException) {
			fprintf(stderr, "Can't compute. `x` is too large.\n");
		}
		return NO_EXCEPTION;
	}

	fprintf(stdout, "Sum of numbers from 1 to %ld: %ld\n", x, result);
	return NO_EXCEPTION;
}

error_t print_factorial(long x) {
	error_t error;
	long result;

	if (FAILED((error = mth_factorial((int)x, &result)))) {
		if (error.code == OverflowException) {
			fprintf(stderr, "Can't compute factorial. Number is too large.\n");
		} else if (error.code == IllegalArgumentException) {
			fprintf(stderr, "Invalid input: %s\n", error.message);
		}
		return NO_EXCEPTION;
	}

	fprintf(stdout, "Factorial of %ld is %ld\n", x, result);
	return NO_EXCEPTION;
}
