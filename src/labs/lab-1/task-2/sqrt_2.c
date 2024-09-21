#include <float.h>
#include <math.h>

#include "lib/error.h"
#include "lib/mth.h"
#include "tasks.h"

double sqrt_2_equation(double x) { return x * x - 2; }

double compute_sqrt_2_lim(double eps) {
	double current = -0.5;
	double next = current - (current * current) / 2 + 1;

	while (fabs(current - next) >= eps) {
		current = next;
		next = current - (current * current) / 2 + 1;
	}

	return next;
}

double compute_sqrt_2_series(double eps) {
	double term = pow(2, pow(2, -2));
	double product = term;

	for (int k = 3; fabs(1 - term) > eps; ++k) {
		term = pow(2, pow(2, -k));
		product *= term;
	}

	return product;
}

double compute_sqrt_2_eq(double eps) {
	double value;

	error_t error = mth_dichotomy(&sqrt_2_equation, -1.0, 2.0, eps, &value);
	if (error != ERROR_SUCCESS) return DBL_MIN;

	return value;
}

