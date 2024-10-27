#include "task.h"

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

bool polynomial_shift(double eps, double a, double** output, int n, ...) {
	/* Given a polynomial P(x) = f_0 + f_1 x + f_2 x^2 + ... + f_n x^n.
	 * Rewrite P(x) such that Q(x+a) = P(x) =
	 *    = g_0 + g_1 (x-a) + g_2 (x-a)^2 + ... + g_n (x-a)^n
	 * ----------------------------------------------------------------
	 * Let x = t+a, t = x-a.
	 * Then P(x) = P(t+a) = f_0 + f_1 (t+a) + f_2 (t+a)^2 + ... + f_n (t+a)^n.
	 *
	 * Expanding (t+a)^k with the Newton's binomial yields:
	 *
	 * P(t+a) = f_0 + f_1 * ∑ [k=0, 1] (C(1, k) t^k a^(1-k))
	 *              + f_2 * ∑ [k=0, 2] (C(2, k) t^k a^(2-k))
	 *              + ...
	 *              + f_n * ∑ [k=0, n] (C(n, k) t^k a^(n-k))
	 *
	 * Consider g_i (x-a)^i = g_i t^i.
	 *
	 * Collecting the coefficients of t^i:
	 * f_i + f_i+1 C(i+1, 1) a + f_i+2 C(i+2, 2) a^2 + ...
	 *
	 * yields g_i = ∑ [k=i, n] f_k C(k, i) a^(k-i).
	 * */

	// Use Pascal's triangle to compute C(n, k) faster.
	int** C = (int**)calloc(n + 1, sizeof(int*));
	if (C == NULL) return false;

	for (int i = 0; i < n + 1; ++i) {
		C[i] = (int*)calloc(n + 1, sizeof(int*));

		if (C[i] == NULL) {
			for (int j = 0; j < n + 1; ++j) free(C[j]);
			free(C);

			return false;
		}

		C[i][0] = 1;
	}

	for (int i = 1; i < n + 1; ++i) {
		for (int j = 1; j < n + 1; ++j) {
			C[i][j] = C[i - 1][j - 1] + C[i - 1][j];
		}
	}

	*output = (double*)calloc(n + 1, sizeof(double));
	if (*output == NULL) {
		for (int i = 0; i < n + 1; ++i) free(C[i]);
		free(C);

		return false;
	}

	double* f = (double*)calloc(n + 1, sizeof(double));

	va_list args;
	va_start(args, n);

	for (int i = 0; i != n + 1; ++i) {
		f[i] = va_arg(args, double);
	}

	va_end(args);

	for (int i = 0; i < n + 1; ++i) {
		for (int k = i; k < n + 1; ++k) {
			(*output)[i] += f[k] * C[k][i] * pow(a, k - i);
		}
	}

	for (int i = 0; i < n + 1; ++i) free(C[i]);
	free(C);
	free(f);

	return true;
}
