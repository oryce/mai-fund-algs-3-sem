#include <math.h>

#include "lib/error.h"
#include "lib/mth.h"

double f_1_(double x) { return 4 - exp(x) - 2 * (x * x); }

double f_2_(double x) { return tan(x); }

double f_3_(double x) { return sin(1 / x); }

error_t main_(void) {
	error_t error;

	double x_1_0_, x_1_1_, x_1_2_;
	double x_2_0_, x_2_1_, x_2_2_;
	double x_3_0_, x_3_1_, x_3_2_;

	error = mth_dichotomy(&f_1_, -2.0, 0.0, 1e-5, &x_1_0_);
	if (FAILED(error)) PASS(error);
	error = mth_dichotomy(&f_1_, -2.0, 0.0, 1e-9, &x_1_1_);
	if (FAILED(error)) PASS(error);
	error = mth_dichotomy(&f_1_, -2.0, 0.0, 1e-3, &x_1_2_);
	if (FAILED(error)) PASS(error);

	error = mth_dichotomy(&f_2_, -3.5, -2.5, 1e-5, &x_2_0_);
	if (FAILED(error)) PASS(error);
	error = mth_dichotomy(&f_2_, -3.5, -2.5, 1e-12, &x_2_1_);
	if (FAILED(error)) PASS(error);
	error = mth_dichotomy(&f_2_, -3.5, -2.5, 1e-3, &x_2_2_);
	if (FAILED(error)) PASS(error);

	error = mth_dichotomy(&f_3_, 0.3, 0.4, 1e-5, &x_3_0_);
	if (FAILED(error)) PASS(error);
	error = mth_dichotomy(&f_3_, 0.3, 0.4, 1e-12, &x_3_1_);
	if (FAILED(error)) PASS(error);
	error = mth_dichotomy(&f_3_, 0.3, 0.4, 1e-3, &x_3_2_);
	if (FAILED(error)) PASS(error);

	printf("eps = 1e-5, x = %.16lf\n", x_1_0_);
	printf("eps = 1e-9, x = %.16lf\n", x_1_1_);
	printf("eps = 1e-3, x = %.16lf\n", x_1_2_);

	printf("eps = 1e-5, x = %.16lf\n", x_2_0_);
	printf("eps = 1e-12, x = %.16lf\n", x_2_1_);
	printf("eps = 1e-3, x = %.16lf\n", x_2_2_);

	printf("eps = 1e-5, x = %.16lf\n", x_3_0_);
	printf("eps = 1e-12, x = %.16lf\n", x_3_1_);
	printf("eps = 1e-3, x = %.16lf\n", x_3_2_);

	return NO_EXCEPTION;
}

int main(void) {
	error_t error = main_();
	if (FAILED(error)) {
		error_print(error);
		return error.code;
	}
}
