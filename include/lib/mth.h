#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "error.h"

/**
 * Computes n^power using binary exponentiation.
 *
 * @return `ERROR_INVALID_PARAMETER` if power < 0.
 */
error_t mth_long_pow(long n, int power, long* out);

/**
 * Computes n^power using binary exponentiation.
 *
 * @return `ERROR_INVALID_PARAMETER` if power < 0.
 */
error_t mth_double_pow(double n, int power, double* out);

/**
 * Computes the limit of sequence f(n) with an error margin of `eps`.
 */
double mth_sequence_limit(double f(int), double eps);

/**
 * Computes n! -- factorial of `n`.
 *
 * @return `ERROR_INVALID_PARAMETER` -- if n < 0;
 *   	   `ERROR_OVERFLOW` -- if the result is too large.
 */
error_t mth_factorial(int n, long* out);

/**
 * Finds the root of f(x) using binary search (dichotomy).
 *
 * @param a lower bound
 * @param b upper bound
 * @param eps error margin
 *
 * @return `ERROR_INVALID_PARAMETER` if the function does not have a root in [a; b].
 */
error_t mth_dichotomy(double f(double), double a, double b, double eps, double* out);

/**
 * Computes a prime sieve.
 *
 * @param isPrime resulting prime sieve; an array with size of at least `n`
 * @param n amount of numbers to check
 * @param zeroPrimes whether the sieve should be zeroed -- useful when expanding
 *
 * @return `ERROR_INVALID_PARAMETER` if n < 0.
 */
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

/** Finds a maximum value between two numbers. */
inline static int mth_int_max(int a, int b) { return a > b ? a : b; }

/** Finds a minimum value between two numbers. */
inline static int mth_int_min(int a, int b) { return a < b ? a : b; }

/** Finds a maximum value between two numbers. */
inline static long mth_long_max(long a, long b) { return a > b ? a : b; }

/** Finds a maximum value between two numbers. */
inline static long mth_long_min(long a, long b) { return a < b ? a : b; }

/**
 * Returns a pseudo-random value in [a; b).
 *
 * The RNG should be seeded first.
 */
inline static long mth_rand(long min, long max) {
	return rand() % (max - min) + min;  // NOLINT(*-msc50-cpp)
}
