#include "tasks.h"

#include <stdio.h>

#include "lib/mth.h"

error_t print_divisible(long x) {
	if (x <= 0) return ERR_INVVAL;

	if (x > 100) {
		fprintf(stdout, "No numbers in [1; 100] are divisible by %ld.\n", x);
		return 0;
	}

	for (long i = x; i <= 100; i += x) {
		fprintf(stdout, "%ld is divisible by %ld\n", i, x);
	}

	return 0;
}

error_t is_prime_(long x, bool* result) {
	if (x <= 0) return ERR_INVVAL;

	if (x == 2 || x == 3) {
		*result = true;
		return 0;
	} else if (x == 1 || x % 2 == 0 || x % 3 == 0) {
		*result = false;
		return 0;
	}

	for (long i = 5; i * i <= x; i += 6) {
		if (x % i == 0 || x % (i + 2) == 0) {
			*result = false;
			return 0;
		}
	}

	*result = true;
	return 0;
}

error_t print_is_prime(long x) {
	bool result;

	error_t error = is_prime_(x, &result);
	if (error) return error;

	fprintf(stdout, "%ld is a %s number", x, result ? "prime" : "composite");
	return 0;
}

error_t print_hex(long x) {
	if (x < 0) x = -x;

	char digits[16];  // log_16(LONG_MAX): max length of number
	int length = 0;

	while (x != 0) {
		long r = x % 16;
		x /= 16;

		digits[length++] = (char)(r < 10 ? '0' + r : 'a' + (r - 10));
	}

	for (int i = length - 1; i >= 0; --i) {
		fprintf(stdout, "%c ", digits[i]);
	}

	fprintf(stdout, "\n");
	return 0;
}

error_t print_powers(long x) {
	if (x < 1 || x > 10) {
		fprintf(stderr, "Invalid `x`: must be in [1; 10].\n");
		return 0;
	}

	fprintf(stdout, "Base  | Power | Base^Power\n");

	for (int base = 1; base != 10 + 1; ++base) {
		long result = 1;
		for (int power = 1; power != x + 1; ++power) {
			result *= base;
			fprintf(stdout, "%5d | %5d | %10ld\n", base, power, result);
		}
	}

	return 0;
}

// sqrt(LONG_MAX) * sqrt(2)
const long SUM_LIMIT = 4294967296;

error_t arithmetic_progression_(long n, long* result) {
	if (n < 1) return ERR_INVVAL;
	if (n >= SUM_LIMIT) return ERR_OVERFLOW;

	*result = (long)((long double)(1 + n) / 2 * (long double)n);
	return 0;
}

error_t print_sums(long x) {
	long result;
	error_t error = arithmetic_progression_(x, &result);

	if (error) {
		if (error == ERR_INVVAL)
			fprintf(stderr, "Invalid input\n");
		else if (error == ERR_OVERFLOW)
			fprintf(stderr, "Can't compute. `x` is too large.\n");
		return 0;
	}

	fprintf(stdout, "Sum of numbers from 1 to %ld: %ld\n", x, result);
	return 0;
}

error_t print_factorial(long x) {
	unsigned long result;
	error_t error = mth_factorial((int)x, &result);

	if (error) {
		if (error == ERR_OVERFLOW)
			fprintf(stderr, "Can't compute factorial. Number is too large.\n");
		else if (error == ERR_INVVAL)
			fprintf(stderr, "Invalid input\n");
		return 0;
	}

	fprintf(stdout, "Factorial of %ld is %ld\n", x, result);
	return 0;
}
