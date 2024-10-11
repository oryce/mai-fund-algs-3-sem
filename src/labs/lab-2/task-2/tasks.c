#include "tasks.h"

#include <math.h>

#include "lib/error.h"

error_t geometric_mean(double* result, int n, ...) {
	if (result == NULL || n < 1) {
		return ERROR_INVALID_PARAMETER;
	}

	va_list args;
	va_start(args, n);

	double product = 1;
	for (int i = 0; i != n; ++i) {
		product *= va_arg(args, double);
	}

	va_end(args);

	if (isinf(product)) {
		return ERROR_OVERFLOW;
	}

	*result = pow(product, 1.0 / n);
	return isnan(*result) ? ERROR_OVERFLOW : ERROR_SUCCESS;
}

error_t bin_exp(double* out, double number, int power) {
	error_t error;

	if (power < 0) {
		double res;
		error = bin_exp(&res, number, -power);
		if (error) return error;

		*out = 1.0 / res;
		return isnan(*out) ? ERROR_UNDERFLOW : ERROR_SUCCESS;
	} else if (power == 0) {
		*out = 1;
		return ERROR_SUCCESS;
	}

	double res;
	error = bin_exp(&res, number, power / 2);
	if (error) return error;

	if (power % 2) {
		*out = res * res * number;
	} else {
		*out = res * res;
	}
	return isinf(*out) ? ERROR_OVERFLOW : ERROR_SUCCESS;
}
