#include "tasks.h"

#include <math.h>

error_t compute_series_a(double eps, double x, double* out) {
	double term = 1;
	double sum = term;

	for (int k = 1; fabs(term) > eps; ++k) {
		term *= x / k;
		sum += term;
		if (isinf(sum)) return ERROR_DIVERGING;
	}

	*out = sum;
	return ERROR_SUCCESS;
}

error_t compute_series_b(double eps, double x, double* out) {
	double term = 1;
	double sum = term;

	for (int k = 1; fabs(term) > eps; ++k) {
		int n = k - 1;
		// solving S(x, n) * k = S(x, n + 1) for k and simplifying the result here
		term *= -(x * x) / ((2 * n + 1) * (2 * n + 2));

		sum += term;
		if (isinf(sum)) return ERROR_DIVERGING;
	}

	*out = sum;
	return ERROR_SUCCESS;
}

error_t compute_series_c(double eps, double x, double* out) {
	double term = 1;
	double sum = term;

	for (int k = 1; fabs(term) > eps; ++k) {
		int n = k - 1;
		term *= (9 * (n + 1) * (n + 1) * x * x) / ((3 * n + 1) * (3 * n + 2));

		sum += term;
		if (isinf(sum)) return ERROR_DIVERGING;
	}

	*out = sum;
	return ERROR_SUCCESS;
}

error_t compute_series_d(double eps, double x, double* out) {
	double term = -(x * x) / 2;
	double sum = term;

	for (int k = 2; fabs(term) > eps; ++k) {
		int n = k - 1;
		term *= -((2 * n + 1) * x * x) / (2 * (n + 1));

		sum += term;
		if (isinf(sum)) return ERROR_DIVERGING;
	}

	*out = sum;
	return ERROR_SUCCESS;
}
