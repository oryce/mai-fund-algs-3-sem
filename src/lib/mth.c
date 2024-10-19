#include "lib/mth.h"

#include <limits.h>
#include <math.h>

#define MTH_MAX_INTEGRAL_PARTITIONS (INT_MAX / 2)

#define MTH_BIN_POW(TYPE, NAME)    \
	TYPE NAME(TYPE n, int power) { \
		TYPE result = 1;           \
                                   \
		while (power > 0) {        \
			if (power & 1) {       \
				result *= n;       \
			}                      \
                                   \
			n = n * n;             \
			power = power >> 1;    \
		}                          \
                                   \
		return result;             \
	}

MTH_BIN_POW(long, mth_long_bin_pow_)
MTH_BIN_POW(double, mth_double_bin_pow_)

error_t mth_long_pow(long n, int power, long* out) {
	if (power < 0) {
		THROW(IllegalArgumentException, "negative power is invalid for `long`");
	}

	*out = mth_long_bin_pow_(n, power);
	return NO_EXCEPTION;
}

error_t mth_double_pow(double n, int power, double* out) {
	if (power < 0) return mth_double_pow(1. / n, power, out);

	*out = mth_double_bin_pow_(n, power);
	if (isinf(*out)) THROW(OverflowException, "overflow in power result");
	if (isnan(*out)) THROW(UnderflowException, "underflow in power result");

	return NO_EXCEPTION;
}

const char* mth_error_to_string(error_code_t error) {
	switch (error) {
		case IntegralException:
			return "IntegralException";
		case DivergingException:
			return "DivergingException";
		default:
			return NULL;
	}
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

error_t mth_factorial(int n, long* out) {
	if (n < 0) THROW(IllegalArgumentException, "`n` may not be less than zero");

	long result = 1;

	if (n > 1) {
		for (int i = 2; i != (n + 1); ++i) {
			result *= i;
			if (result < 0) {
				THROW(OverflowException, "long overflow in `result`");
			}
		}
	}

	*out = result;
	return NO_EXCEPTION;
}

error_t mth_dichotomy(double f(double), double a, double b, double eps, double* out) {
	double l = f(a);
	double r = f(b);

	if (l * r > 0) {
		THROW(IllegalArgumentException, "`f` must reach zero in [a; b]");
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
	return NO_EXCEPTION;
}

error_t mth_prime_sieve(bool* isPrime, int n, bool zeroPrimes) {
	if (n < 0) THROW(IllegalArgumentException, "invalid number count");

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

	return NO_EXCEPTION;
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
			THROW(IntegralException, "too many partitions");
		}

		current = next;
		next = trapezoidal_integral_(f, a, b, n * 2);
	}

	*out = next;
	return NO_EXCEPTION;
}
