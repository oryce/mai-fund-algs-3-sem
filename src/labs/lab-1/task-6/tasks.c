#include "tasks.h"

#include <math.h>

#include "lib/mth.h"

double func_a(double x) {
	if (x == 0) return 1;
	return log(1 + x) / x;
}

double func_b(double x) {
	return exp(-(x * x) / 2);
}

double func_c(double x) {
	if (x == 1) return 1;
	return log(1 / (1 - x));
}

double func_d(double x) {
	if (x == 0) return 1;
	return pow(x, x);
}

error_t compute_integral_a(double eps, double* out) { return mth_integral(&func_a, 0, 1, eps, out); }

error_t compute_integral_b(double eps, double* out) { return mth_integral(&func_b, 0, 1, eps, out); }

error_t compute_integral_c(double eps, double* out) { return mth_integral(&func_c, 0, 1, eps, out); }

error_t compute_integral_d(double eps, double* out) { return mth_integral(&func_d, 0, 1, eps, out); }
