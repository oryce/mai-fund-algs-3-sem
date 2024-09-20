#pragma once

#include <stdbool.h>

#include "error.h"

error_t mth_long_pow(long n, int power, long* out);

error_t mth_double_pow(double n, int power, double* out);

double mth_sequence_limit(double sequence(int), double eps);

error_t mth_factorial(int n, long* out);

error_t mth_combinations(int n, int k, long* out);

error_t mth_dichotomy(double equation(double), double a, double b, double eps, double* out);

error_t mth_prime_sieve(bool* isPrime, int n, bool zeroPrimes);
