#include "tasks.h"

error_t main_(void) {
	error_t error;

	double mean;
	if (FAILED((error = task_geometric_mean(&mean, 0)))) {
		PASS(error);
	}

	printf("Mean: %lf\n", mean);

	double a, b;
	if (FAILED((error = task_bin_exp(&a, 5, 10)))) {
		PASS(error);
	}
	if (FAILED((error = task_bin_exp(&b, 0.5, -10)))) {
		PASS(error);
	}

	printf("5^10 = %lf\n", a);
	printf("0.5^(-10) = %lf\n", b);

	return NO_EXCEPTION;
}

int main(void) {
	error_t error = main_();
	if (FAILED(error)) {
		error_print(error);
		return (int)error.code;
	}
}
