#include "lib/mth.h"

#include <limits.h>
#include <math.h>
#include <stdckdint.h>

#define MTH_MAX_INTEGRAL_PARTITIONS (INT_MAX / 2)

bool mth_long_bin_pow_(long n, int power, long* out) {
	if (!out) return false;
	long result = 1;

	while (power > 0) {
		if (power & 1) {
			// result *= n
			if (ckd_mul(&result, result, n)) return false;
		}

		// n = n * n
		if (ckd_mul(&n, n, n)) return false;
		power = power >> 1;
	}

	*out = result;
	return true;
}

double mth_double_bin_pow_(double n, int power) {
	double result = 1;

	while (power > 0) {
		if (power & 1) {
			result *= n;
		}

		n = n * n;
		power = power >> 1;
	}

	return result;
}

error_t mth_long_pow(long n, int power, long* out) {
	if (power < 0) return ERR_INVVAL;
	return mth_long_bin_pow_(n, power, out);
}

error_t mth_double_pow(double n, int power, double* out) {
	if (!out) return ERR_INVVAL;
	if (power < 0) return mth_double_pow(1. / n, power, out);

	*out = mth_double_bin_pow_(n, power);
	if (isinf(*out)) return ERR_OVERFLOW;
	if (isnan(*out)) return ERR_UNDERFLOW;

	return 0;
}

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

error_t mth_factorial(unsigned int n, unsigned long* out) {
	if (!out) return ERR_INVVAL;

	long result = 1;

	if (n > 1) {
		for (int i = 2; i != (n + 1); ++i) {
			// result *= i
			if (ckd_mul(&result, result, i)) return ERR_OVERFLOW;
		}
	}

	*out = result;
	return 0;
}

error_t mth_dichotomy(double f(double), double a, double b, double eps, double* out) {
	double l = f(a);
	double r = f(b);

	if (l * r > 0) {
		return ERR_INVVAL;
	}

	while (fabs(a - b) > eps) {
		double mid = (a + b) / 2;

		if (f(mid) * l > 0) {
			a = mid;
		} else {
			b = mid;
		}
	}

	*out = (a + b) / 2;
	return 0;
}

error_t mth_prime_sieve(bool* isPrime, int n, bool zeroPrimes) {
	if (n < 0) return ERR_INVVAL;

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

	return 0;
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
double trapezoidal_integral_(double f(double x), double a, double b, int n) {
	double h = (b - a) / n;  // Partition width; divided evenly
	double sum = 0;

	for (int k = 0; k < n; ++k) {
		sum += (f(a + k * h) + f(a + (k + 1) * h)) / 2 * h;
	}

	return sum;
}

error_t mth_integral(double f(double x), double a, double b, double eps, double* out) {
	int n = 2;  // Number of partitions

	double current = trapezoidal_integral_(f, a, b, n);
	double next = trapezoidal_integral_(f, a, b, n * 2);

	while (fabs(current - next) > eps) {
		n *= 2;
		if (n > MTH_MAX_INTEGRAL_PARTITIONS) {
			return ERR_M_INTEGRAL;
		}

		current = next;
		next = trapezoidal_integral_(f, a, b, n * 2);
	}

	*out = next;
	return 0;
}

unsigned long mth_gcd(unsigned long a, unsigned long b) {
	/* https://en.wikipedia.org/wiki/Binary_GCD_algorithm */

	unsigned long t = a | b;
	if (a == 0 || b == 0) return t;

	int g = __builtin_ctz(t);

	while (a != 0) {
		a >>= __builtin_ctz(a);
		b >>= __builtin_ctz(b);
		if (a >= b)
			a = (a - b) / 2;
		else
			b = (b - a) / 2;
	}

	return (b << g);
}
