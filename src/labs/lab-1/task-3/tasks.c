#include "tasks.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#include "lib/conv.h"

#define ZERO(a, eps) (fabs(a) < eps)
#define EQUAL(a, b, eps) (fabs((a) - (b)) < eps)
#define LESS(a, b, eps) ((a) - (b) < eps)

void solve_with_coefficients(double eps, double a, double b, double c) {
	fprintf(stdout, "a = %f, b = %f, c = %f\n", a, b, c);

	if (ZERO(a, eps) && ZERO(b, eps) && ZERO(c, eps)) {
		fprintf(stdout, "  infinite solutions\n");
	} else if ((ZERO(a, eps) && ZERO(b, eps) /* && c != 0 */) || LESS(b * b, 4 * a * c, eps)) {
		fprintf(stdout, "  no solutions\n");
	} else if (EQUAL(b * b, 4 * a * c, eps)) {
		double x = -b / 2 * a;
		fprintf(stdout, "  x = %f\n", x);
	} else if (ZERO(a, eps)) {
		double x = -c / b;
		fprintf(stdout, "  x = %f\n", x);
	} else {
		double sqrt_d = sqrt(b * b - 4 * a * c);
		double x1 = (-b - sqrt_d) / (2 * a);
		double x2 = (-b + sqrt_d) / (2 * a);
		fprintf(stdout, "  x1 = %f, x2 = %f\n", x1, x2);
	}
}

error_t solve_quadratic_equation(int argc, char** argv) {
	// <prog> <flag> <eps> <a> <b> <c>
	if (argc != 6) return ERROR_INVALID_PARAMETER;

	error_t error;
	double eps;
	double a, b, c;

	error = str_to_double(argv[2], &eps);
	if (error != ERROR_SUCCESS) return error;
	if (eps <= 0) return ERROR_INVALID_PARAMETER;
	error = str_to_double(argv[3], &a);
	if (error != ERROR_SUCCESS) return error;
	error = str_to_double(argv[4], &b);
	if (error != ERROR_SUCCESS) return error;
	error = str_to_double(argv[5], &c);
	if (error != ERROR_SUCCESS) return error;

	solve_with_coefficients(eps, a, b, c);
	solve_with_coefficients(eps, a, c, b);
	solve_with_coefficients(eps, b, a, c);
	solve_with_coefficients(eps, b, c, a);
	solve_with_coefficients(eps, c, a, b);
	solve_with_coefficients(eps, c, b, a);

	return ERROR_SUCCESS;
}

error_t check_is_divisible(int argc, char** argv) {
	// <prog> <flag> <a> <b>
	if (argc != 4) return ERROR_INVALID_PARAMETER;

	error_t error;
	long a, b;

	error = str_to_long(argv[2], &a);
	if (error != ERROR_SUCCESS) return error;
	error = str_to_long(argv[3], &b);
	if (error != ERROR_SUCCESS) return error;

	if (a == 0 || b == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	if (a % b == 0) {
		fprintf(stdout, "%ld is divisible by %ld\n", a, b);
	} else {
		fprintf(stdout, "%ld is not divisible by %ld\n", a, b);
	}

	return ERROR_SUCCESS;
}

error_t check_is_right_triangle(int argc, char** argv) {
	// <prog> <flag> <eps> <a> <b> <c>
	if (argc != 6) return ERROR_INVALID_PARAMETER;

	error_t error;
	double eps;
	double a, b, c;

	error = str_to_double(argv[2], &eps);
	if (error != ERROR_SUCCESS) return error;
	if (eps <= 0) return ERROR_INVALID_PARAMETER;
	error = str_to_double(argv[3], &a);
	if (error != ERROR_SUCCESS) return error;
	error = str_to_double(argv[4], &b);
	if (error != ERROR_SUCCESS) return error;
	error = str_to_double(argv[5], &c);
	if (error != ERROR_SUCCESS) return error;

	if (a == 0 || b == 0 || c == 0) {
		return ERROR_INVALID_PARAMETER;
	}

	bool triangle = LESS(a, b + c, eps) && LESS(b, a + c, eps) && LESS(c, a + b, eps);
	bool right_triangle =
	    EQUAL(a * a + b * b, c * c, eps) || EQUAL(a * a + c * c, b * b, eps) || EQUAL(b * b + c * c, a * a, eps);

	if (triangle && right_triangle) {
		fprintf(stdout, "%f %f %f can build a right triangle\n", a, b, c);
	} else {
		fprintf(stdout, "%f %f %f cannot build a right triangle\n", a, b, c);
	}

	return ERROR_SUCCESS;
}
