#include "tasks.h"

error_t main_(void) {
	error_t error;

	double mean;

	error = task_geometric_mean(&mean, 4, 1.0, 2.0, 3.0, 4.0);
	if (error) return error;

	printf("Mean: %lf\n", mean);

	double a, b;

	error = task_bin_exp(&a, 5, 10);
	if (error) return error;
	error = task_bin_exp(&b, 0.5, -10);
	if (error) return error;

	printf("5^10 = %lf\n", a);
	printf("0.5^(-10) = %lf\n", b);

	return 0;
}

int main(void) {
	error_t error = main_();
	if (error) {
		error_print(error);
		return error;
	}
}
