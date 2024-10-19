#include "tasks.h"

#include <math.h>

#include "lib/error.h"

error_t task_geometric_mean(double* result, int n, ...) {
	if (result == NULL) THROW(IllegalArgumentException, "`result` may not be null");
	if (n < 1) THROW(IllegalArgumentException, "at least one number must be provided");

	va_list args;
	va_start(args, n);

	double product = 1;
	for (int i = 0; i != n; ++i) {
		product *= va_arg(args, double);
	}

	va_end(args);

	if (isinf(product)) {
		THROW(OverflowException, "double overflow in number product");
	}

	*result = pow(product, 1.0 / n);
	if (isnan(*result)) {
		THROW(UnderflowException, "double underflow in geometric mean");
	}

	return NO_EXCEPTION;
}

error_t task_bin_exp(double* out, double number, int power) {
	error_t status;

	if (power < 0) {
		double result;
		if (FAILED((status = task_bin_exp(&result, number, -power)))) {
			PASS(status);
		}

		*out = 1.0 / result;
		if (isnan(*out)) {
			THROW(OverflowException, "double underflow in negative exponent");
		}

		return NO_EXCEPTION;
	} else if (power == 0) {
		*out = 1;
		return NO_EXCEPTION;
	}

	double result;
	if (FAILED((status = task_bin_exp(&result, number, power / 2)))) {
		PASS(status);
	}

	if (power % 2) {
		*out = result * result * number;
	} else {
		*out = result * result;
	}
	if (isinf(*out)) {
		THROW(OverflowException, "double overflow in exponent");
	}

	return NO_EXCEPTION;
}
