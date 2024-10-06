#include <float.h>
#include <math.h>

#include "lib/mth.h"
#include "tasks.h"

double exp_sequence(int n) {
	double value;

	error_t error = mth_double_pow(1 + 1 / (double)n, n, &value);
	if (error != ERROR_SUCCESS) return DBL_MIN;

	return value;
}

double exp_equation(double x) {
	return log(x) - 1;
}

double compute_exp_lim(double eps) { return mth_sequence_limit(&exp_sequence, eps); }

double compute_exp_series(double eps) {
	double term = 1;
	double sum = term;

	for (int n = 2; term > eps; ++n) {
		term /= n;
		sum += term;
	}

	return 1 + sum;
}

double compute_exp_eq(double eps) {
	double value;

	error_t error = mth_dichotomy(&exp_equation, 2.0, 3.0, eps, &value);
	if (error != ERROR_SUCCESS) return DBL_MIN;

	return value;
}
