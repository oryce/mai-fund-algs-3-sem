#include "tasks.h"

int main(void) {
	error_t error;

	double mean;
	error = geometric_mean(&mean, 5, 1.0, 2.0, 3.0, 4.0, 5.0);
	if (error) {
		error_print(error);
		return (int)-error;
	}

	printf("Mean: %lf\n", mean);

	double a, b;
	error = bin_exp(&a, 5, 10);
	if (error) {
		error_print(error);
		return (int)-error;
	}
	error = bin_exp(&b, 0.5, -10);
	if (error) {
		error_print(error);
		return (int)-error;
	}

	printf("5^10 = %lf\n", a);
	printf("0.5^(-10) = %lf\n", b);
}
