#include "lib/mth.h"

#include <math.h>

error_t mth_long_pow(long n, int power, long* out) {
	if (power == 0) {
		*out = 1;
		return ERROR_SUCCESS;
	} else if (power < 0) {
		return ERROR_INVALID_PARAMETER;
	}

	long result = n;

	for (int i = 1; i != power; ++i) {
		result *= n;
		if (result < 0) return ERROR_OVERFLOW;
	}

	*out = result;
	return ERROR_SUCCESS;
}

error_t mth_double_pow(double n, int power, double* out) {
	if (power == 0) {
		*out = 1.0;
		return ERROR_SUCCESS;
	} else if (power < 0) {
		return ERROR_INVALID_PARAMETER;
	}

	double result = n;

	for (int i = 1; i != power; ++i) {
		result *= n;
		if (result < 0) return ERROR_OVERFLOW;
	}

	*out = result;
	return ERROR_SUCCESS;
}

double mth_sequence_limit(double sequence(int), double eps) {
	int n = 1;

	double current = sequence(n);
	double next = sequence(n + 1);

	while (fabs(current - next) >= eps) {
		++n;

		current = next;
		next = sequence(n + 1);
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

error_t mth_combinations(int n, int k, long* out) {
	if (n < 0 || k < 0) return ERROR_INVALID_PARAMETER;

	long n_fact, k_fact, n_k_fact;
	error_t error;

	error = mth_factorial(n, &n_fact);
	if (error != ERROR_SUCCESS) return error;
	error = mth_factorial(k, &k_fact);
	if (error != ERROR_SUCCESS) return error;
	error = mth_factorial(n - k, &n_k_fact);
	if (error != ERROR_SUCCESS) return error;

	*out = n_fact / (n_k_fact * k_fact);
	return ERROR_SUCCESS;
}

error_t mth_dichotomy(double equation(double), double a, double b, double eps, double* out) {
	double l = equation(a);
	double r = equation(b);

	if (l * r > 0) return ERROR_INVALID_PARAMETER;

	while (fabs(a - b) > eps) {
		double mid = (a + b) / 2;

		if (equation(mid) * l > 0) {
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
