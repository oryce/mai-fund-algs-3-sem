#include "tasks.h"

#include <math.h>

#include "lib/error.h"

error_t task_geometric_mean(double* result, int n, ...) {
	if (!result || n < 1) return ERR_INVVAL;

	va_list args;
	va_start(args, n);

	double product = 1;

	for (int i = 0; i != n; ++i) {
		product *= va_arg(args, double);
	}

	va_end(args);

	if (isinf(product)) return ERR_OVERFLOW;

	*result = pow(product, 1.0 / n);
	if (isnan(*result)) return ERR_UNDERFLOW;

	return 0;
}

error_t task_bin_exp(double* out, double number, int power) {
	error_t status;

	if (power < 0) {
		double result;

		status = task_bin_exp(&result, number, -power);
		if (status) return status;

		*out = 1.0 / result;
		if (isnan(*out)) return ERR_UNDERFLOW;

		return 0;
	} else if (power == 0) {
		*out = 1;
		return 0;
	}

	double result;

	status = task_bin_exp(&result, number, power / 2);
	if (status) return status;

	if (power % 2)
		*out = result * result * number;
	else
		*out = result * result;
	if (isinf(*out)) return ERR_UNDERFLOW;

	return 0;
}
