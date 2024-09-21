#include <float.h>
#include <math.h>

#include "lib/mth.h"
#include "tasks.h"

double pi_sequence(int n) {
	error_t error;

	long n_fact, two_n_fact, two_to_n;
	long nominator;

	error = mth_factorial(n, &n_fact);
	if (error != ERROR_SUCCESS) return DBL_MIN;
	error = mth_factorial(2 * n, &two_n_fact);
	if (error != ERROR_SUCCESS) return DBL_MIN;

	error = mth_long_pow(2, n, &two_to_n);
	if (error != ERROR_SUCCESS) return DBL_MIN;
	error = mth_long_pow(two_to_n * n_fact, 4, &nominator);
	if (error != ERROR_SUCCESS) return DBL_MIN;
	double denominator = (double)(n * two_n_fact * two_n_fact);
	if (denominator < 0) return DBL_MIN;

	return (double)nominator / denominator;
}

double pi_equation(double x) {
	return sin(x);
}

double compute_pi_lim(double eps) {
	int n = 1;

	double two_n_p_1_sq;
	error_t error;

	double current = pi_sequence(n);
	double next = pi_sequence(n + 1);

	while (fabs(current - next) >= eps) {
		++n;

		error = mth_double_pow(2.0 * n + 1, 2, &two_n_p_1_sq);
		if (error != ERROR_SUCCESS) return DBL_MIN;

		current = next;
		next *= (4.0 * n * (n + 1)) / two_n_p_1_sq;
	}

	return next;
}

double compute_pi_series(double eps) {
	double term = 1;
	double sum = term;

	for (int n = 2; fabs(term) > eps; ++n) {
		term = ((n - 1) % 2 ? -1.0 : 1.0) / (2 * n - 1);
		sum += term;
	}

	return 4 * sum;
}

double compute_pi_eq(double eps) {
	double value;

	error_t error = mth_dichotomy(&pi_equation, 2.5, 3.5, eps, &value);
	if (error != ERROR_SUCCESS) return DBL_MIN;

	return value;
}
