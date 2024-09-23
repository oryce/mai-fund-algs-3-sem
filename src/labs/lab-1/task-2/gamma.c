#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib/mth.h"
#include "tasks.h"

double gamma_sequence(int n) {
	double sum = 0;

	for (int k = 1; k != (n + 1); ++k) {
		sum += 1.0 / k;
	}

	return sum - log(n);
}

double gamma_eq_sequence(const bool* isPrime, int t) {
	double product = 1;

	for (int p = 2; p != (t + 1); ++p) {
		if (isPrime[p]) {
			product *= (p - 1.0) / p;
		}
	}

	return log(t) * product;
}

double compute_gamma_lim(double eps) { return mth_sequence_limit(&gamma_sequence, eps); }

double compute_gamma_series(double eps) {
	double term = 0.5;
	double sum = term;

	double floor_sqrt_sq;
	error_t error;

	for (int k = 3; term == 0 || fabs(term) > eps; ++k) {
		error = mth_double_pow(floor(sqrt(k)), 2, &floor_sqrt_sq);
		if (error != ERROR_SUCCESS) return DBL_MIN;

		term = 1.0 / floor_sqrt_sq - 1.0 / k;
		sum += term;
	}

	return -(3.1415 * 3.1415) / 6 + sum;
}

double compute_gamma_eq(double eps) {
	int primes = 15;

	bool* isPrime = (bool*)malloc(sizeof(bool) * (primes + 1));
	if (isPrime == NULL) return DBL_MIN;

	error_t error = mth_prime_sieve(isPrime, primes, true);
	if (error != ERROR_SUCCESS) {
		free(isPrime);
		return DBL_MIN;
	}

	int n = 2;

	double current = gamma_eq_sequence(isPrime, 2);
	double next = gamma_eq_sequence(isPrime, 3);

	while (fabs(current - next) > eps) {
		++n;

		if (n + 1 > primes) {
			// Need to resize the prime sieve
			primes *= 2;

			bool* newPrimes = (bool*)realloc(isPrime, sizeof(bool) * (primes + 1));
			if (newPrimes == NULL) {
				free(isPrime);
				return DBL_MIN;
			}

			isPrime = newPrimes;
			for (int i = n + 1; i != (primes + 1); ++i) {
				isPrime[i] = true;
			}

			error = mth_prime_sieve(isPrime, primes, false);
			if (error != ERROR_SUCCESS) {
				free(isPrime);
				return DBL_MIN;
			}
		}

		current = next;
		next = gamma_eq_sequence(isPrime, n + 1);
	}

	free(isPrime);
	return next;
}
