#include <float.h>
#include <math.h>

#include "lib/mth.h"
#include "tasks.h"

double ln_2_sequence(int n) { return n * (pow(2, 1. / n) - 1); }

double ln_2_equation(double x) { return exp(x) - 2; }

double compute_ln_2_lim(double eps) { return mth_sequence_limit(&ln_2_sequence, eps); }

double compute_ln_2_series(double eps) {
	double term = 1;
	double sum = term;

	for (int n = 2; fabs(term) > eps; ++n) {
		term = ((n - 1) % 2 ? -1.0 : 1.0) / n;
		sum += term;
	}

	return sum;
}

double compute_ln_2_eq(double eps) {
	double value;

	error_t error = mth_dichotomy(&ln_2_equation, 0.5, 1.0, eps, &value);
	if (error != ERROR_SUCCESS) return DBL_MIN;

	return value;
}
