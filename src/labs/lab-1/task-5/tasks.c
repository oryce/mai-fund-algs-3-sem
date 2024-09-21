#include "tasks.h"

#include <math.h>

#define SERIES_LIMIT 100

error_t compute_series_a(double eps, double x, double* out) {
	double term = 1;
	double sum = term;

	for (int k = 1; fabs(term) > eps; ++k) {
		if (k > SERIES_LIMIT) return ERROR_DIVERGING;

		term *= x / k;
		sum += term;
	}

	*out = sum;
	return ERROR_SUCCESS;
}

error_t compute_series_b(double eps, double x, double* out) {
	double term = 1;
	double sum = term;

	for (int k = 1; fabs(term) > eps; ++k) {
		if (k > SERIES_LIMIT) return ERROR_DIVERGING;

		// solving S(x, n) * k = S(x, n + 1) for k and simplifying the result here
		term *= -(x * x) / ((2 * (k - 1) + 1) * (2 * (k - 1) + 2));
		sum += term;
	}

	*out = sum;
	return ERROR_SUCCESS;
}

error_t compute_series_c(double eps, double x, double* out) {
	double term = 1;
	double sum = term;

	for (int k = 1; fabs(term) > eps; ++k) {
		if (k > SERIES_LIMIT) return ERROR_DIVERGING;

		int n = k - 1;
		term *= (9 * (n + 1) * (n + 1) * x * x) / ((3 * n + 1) * (3 * n + 2));

		sum += term;
	}

	*out = sum;
	return ERROR_SUCCESS;
}

error_t compute_series_d(double eps, double x, double* out) {
	double term = -(x * x) / 2;
	double sum = term;

	for (int k = 2; fabs(term) > eps; ++k) {
		if (k > SERIES_LIMIT) return ERROR_DIVERGING;

		int n = k - 1;
		term *= -((2 * n + 1) * x * x) / (2 * (n + 1));

		sum += term;
	}

	*out = sum;
	return ERROR_SUCCESS;
}
