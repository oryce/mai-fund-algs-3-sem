#include "tasks.h"

#include <stdio.h>

#include "lib/mth.h"

error_t print_divisible(long x) {
	if (x <= 0) return ERROR_INVALID_PARAMETER;

	if (x > 100) {
		fprintf(stdout, "No numbers in [1; 100] are divisible by %ld.\n", x);
		return ERROR_SUCCESS;
	}

	for (long i = x; i <= 100; i += x) {
		fprintf(stdout, "%ld is divisible by %ld\n", i, x);
	}

	return ERROR_SUCCESS;
}

error_t print_is_prime(long x) {
	if (x == 0) return ERROR_INVALID_PARAMETER;

	long n = x;
	if (n < 0) n = -n;

	if (n == 1) {
		fprintf(stdout, "%ld is neither a prime nor a composite number\n", x);
		return ERROR_SUCCESS;
	}

	if (n == 2 || n == 3) {
		fprintf(stdout, "%ld is a prime number\n", x);
		return ERROR_SUCCESS;
	} else if (n % 2 == 0 || n % 3 == 0) {
		fprintf(stdout, "%ld is a composite number\n", x);
		return ERROR_SUCCESS;
	}

	for (long i = 5; i * i <= n; i += 6) {
		if (n % i == 0 || n % (i + 2) == 0) {
			fprintf(stdout, "%ld is a composite number\n", x);
			return ERROR_SUCCESS;
		}
	}

	fprintf(stdout, "%ld is a prime number\n", x);
	return ERROR_SUCCESS;
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
	return ERROR_SUCCESS;
}

error_t print_powers(long x) {
	if (x < 1 || x > 10) return ERROR_INVALID_PARAMETER;

	long result;
	error_t error;

	fprintf(stdout, "Base  | Power | Base^Power\n");

	for (int base = 1; base != 10 + 1; ++base) {
		for (int power = 1; power != x + 1; ++power) {
			error = mth_long_pow(base, power, &result);
			if (error != ERROR_SUCCESS) return error;

			fprintf(stdout, "%5d | %5d | %10ld\n", base, power, result);
		}
	}

	return ERROR_SUCCESS;
}

// sqrt(LONG_MAX) * sqrt(2)
const long SUM_LIMIT = 4294967296;

error_t print_sums(long x) {
	if (x < 1) return ERROR_INVALID_PARAMETER;
	// NB: simplified to sqrt((1 + x) * x) = x
	if (x >= SUM_LIMIT) return ERROR_OVERFLOW;

	long sum = (long)((long double)(1 + x) / 2 * (long double)x);
	fprintf(stdout, "Sum of numbers from 1 to %ld: %ld\n", x, sum);

	return ERROR_SUCCESS;
}

error_t print_factorial(long x) {
	long result;

	error_t error = mth_factorial((int)x, &result);
	if (error != ERROR_SUCCESS) return error;

	fprintf(stdout, "Factorial of %ld is %ld\n", x, result);
	return ERROR_SUCCESS;
}
