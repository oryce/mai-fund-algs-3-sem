#include "lib/mth.h"

#include <limits.h>
#include <math.h>

#define MTH_MAX_INTEGRAL_PARTITIONS (INT_MAX / 2)

#define MTH_BIN_POW(TYPE, NAME)                  \
	error_t NAME(TYPE n, int power, TYPE* out) { \
		if (power < 0) {                         \
			return ERROR_INVALID_PARAMETER;      \
		}                                        \
                                                 \
		TYPE result = 1;                         \
                                                 \
		while (power > 0) {                      \
			if (power & 1) {                     \
				result *= n;                     \
			}                                    \
                                                 \
			n = n * n;                           \
			power = power >> 1;                  \
		}                                        \
                                                 \
		*out = result;                           \
		return ERROR_SUCCESS;                    \
	}

MTH_BIN_POW(long, mth_long_pow)
MTH_BIN_POW(double, mth_double_pow)

double mth_sequence_limit(double f(int), double eps) {
	int n = 1;

	double current = f(n);
	double next = f(n + 1);

	while (fabs(current - next) >= eps) {
		++n;

		current = next;
		next = f(n + 1);
	}

	return next;
}

error_t mth_factorial(int n, long* out) {
	if (n < 0) return ERROR_INVALID_PARAMETER;

	long result = 1;

	if (n > 1) {
		for (int i = 2; i != (n + 1); ++i) {
			result *= i;
			if (result < 0) return ERROR_OVERFLOW;
		}
	}

	*out = result;
	return ERROR_SUCCESS;
}

error_t mth_dichotomy(double f(double), double a, double b, double eps, double* out) {
	double l = f(a);
	double r = f(b);

	if (l * r > 0) return ERROR_INVALID_PARAMETER;

	while (fabs(a - b) > eps) {
		double mid = (a + b) / 2;

		if (f(mid) * l > 0) {
			a = mid;
		} else {
			b = mid;
		}
	}

	*out = (a + b) / 2;
	return ERROR_SUCCESS;
}

error_t mth_prime_sieve(bool* isPrime, int n, bool zeroPrimes) {
	if (n < 0) return ERROR_INVALID_PARAMETER;

	if (zeroPrimes) {
		for (int i = 0; i != (n + 1); ++i) {
			isPrime[i] = true;
		}
	}

	for (int p = 2; p * p <= n; ++p) {
		if (isPrime[p]) {
			for (int i = p * p; i <= n; i += p) {
				isPrime[i] = false;
			}
		}
	}

	return ERROR_SUCCESS;
}

/**
 * Computes an approximate value of the definite integral of f(x) within [a; b]
 * using the Trapezoidal rule, given a partition number.
 *
 * @param f integrated function
 * @param a lower bound
 * @param b upper bound
 * @param n partition number
 *
 * @return approximate integral value
 */
double trapezoidal_integral(double f(double x), double a, double b, int n) {
	double h = (b - a) / n;  // Partition width; divided evenly
	double sum = 0;

	for (int k = 0; k < n; ++k) {
		sum += (f(a + k * h) + f(a + (k + 1) * h)) / 2 * h;
	}

	return sum;
}

error_t mth_integral(double f(double x), double a, double b, double eps, double* out) {
	int n = 2;  // Number of partitions

	double current = trapezoidal_integral(f, a, b, n);
	double next = trapezoidal_integral(f, a, b, n * 2);

	while (fabs(current - next) > eps) {
		n *= 2;
		if (n > MTH_MAX_INTEGRAL_PARTITIONS) return ERROR_INTEGRAL_FAIL;

		current = next;
		next = trapezoidal_integral(f, a, b, n * 2);
	}

	*out = next;
	return ERROR_SUCCESS;
}
