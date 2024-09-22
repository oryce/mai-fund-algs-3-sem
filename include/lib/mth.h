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

/**
 * Computes an approximate value of a definite integral of f(x) in bounds [a; b],
 * given an error margin of `eps`.
 *
 * @param f integral function
 * @param a lower bound
 * @param b upper bound
 * @param eps error margin
 * @param out approximate integral value
 *
 * @return error code; `ERROR_SUCCESS` if the computation succeeded
 */
error_t mth_integral(double f(double x), double a, double b, double eps, double* out);

inline static int mth_int_max(int a, int b) { return a > b ? a : b; }
