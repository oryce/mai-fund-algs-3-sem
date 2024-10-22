#include "task.h"

#include <math.h>
#include <stdarg.h>

#include "lib/collections/vector.h"
#include "lib/mth.h"
#include "lib/jtckdint.h"

bool factorize(int n, vector_i32_t* factors) {
	if (!factors) return false;
	*factors = vector_i32_create();

	int i = 2;

	while (i * i <= n) {
		if (n % i) {
			++i;
		} else {
			n /= i;
			if (!vector_i32_push_back(factors, i)) {
				vector_i32_destroy(factors);
				return false;
			}
		}
	}

	if (n > 1) {
		if (!vector_i32_push_back(factors, n)) {
			vector_i32_destroy(factors);
			return false;
		}
	}

	return true;
}

bool finite_representation(double n, vector_i32_t* radixFactors) {
	/* `n` has a finite representation in radix `r` if the simplified
	 * denominator is only divisible by fractions of `r`. */
	unsigned long num;
	unsigned long den = 1;

	while (n != trunc(n)) {
		n *= 10;
		if (ckd_mul(&den, den, 10)) return false;
	}

	num = (unsigned long)n;

	unsigned long gcd = mth_gcd(num, den);
	num /= gcd;
	den /= gcd;

	int factor0 = 0;

	for (size_t i = 0; i != vector_i32_size(radixFactors); ++i) {
		int factor = *vector_i32_get(radixFactors, i);
		if (factor == factor0) continue;

		while (den % factor == 0) {
			den /= factor;
		}

		factor0 = factor;
	}

	return den == 1;  // Completely reduced
}

bool finite_representations(bool* results, int radix, size_t n, ...) {
	if (!results || radix < 2 || radix > 36 || n == 0) return false;

	vector_i32_t radixFactors;
	if (!factorize(radix, &radixFactors)) return false;

	va_list args;
	va_start(args, n);

	for (size_t i = 0; i != n; ++i) {
		double fraction = va_arg(args, double);

		if (fraction < 0.0 || fraction > 1.0) {
			vector_i32_destroy(&radixFactors);
			va_end(args);

			return false;
		}

		results[i] = finite_representation(fraction, &radixFactors);
	}

	vector_i32_destroy(&radixFactors);
	va_end(args);

	return true;
}
