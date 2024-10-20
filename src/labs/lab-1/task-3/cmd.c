#include "cmd.h"

#include "lib/convert.h"
#include "tasks.h"

void print_quad_eq_(double a, double b, double c, double eps) {
	fprintf(stdout, "a = %lf, b = %lf, c = %lf\n", a, b, c);
	quad_sol_t result = task_quad_eq(a, b, c, eps);

	switch (result.type) {
		case QUAD_INFINITE_SOLUTIONS:
			fprintf(stdout, "  infinite solutions\n");
			break;
		case QUAD_NO_SOLUTIONS:
			fprintf(stdout, "  no solutions\n");
			break;
		case QUAD_SINGLE_SOLUTION:
			fprintf(stdout, "  x = %lf\n", result.solution.single);
			break;
		case QUAD_MULTIPLE_SOLUTIONS:
			fprintf(stdout, "  x1 = %lf, x2 = %lf\n", result.solution.multiple[0], result.solution.multiple[1]);
			break;
	}
}

error_t cmd_quad_eq(int argc, char** argv) {
	// <prog> <flag> <eps> <a> <b> <c>
	if (argc != 6) {
		fprintf(stderr, "Invalid arguments. See usage for more info.\n");
		return NO_EXCEPTION;
	}

	error_t error;
	double eps;
	double a, b, c;

	if (FAILED((error = str_to_double(argv[2], &eps)))) {
		fprintf(stderr, "Invalid `eps`: %s.\n", error.message);
		return NO_EXCEPTION;
	}
	if (eps <= 0) {
		fprintf(stderr, "Invalid `eps`: may not be less or equal to zero.\n");
		return NO_EXCEPTION;
	}
	if (FAILED((error = str_to_double(argv[3], &a)))) {
		fprintf(stderr, "Invalid `a`: %s.\n", error.message);
		return NO_EXCEPTION;
	}
	if (FAILED((error = str_to_double(argv[4], &b)))) {
		fprintf(stderr, "Invalid `b`: %s.\n", error.message);
		return NO_EXCEPTION;
	}
	if (FAILED((error = str_to_double(argv[5], &c)))) {
		fprintf(stderr, "Invalid `c`: %s.\n", error.message);
		return NO_EXCEPTION;
	}

	print_quad_eq_(a, b, c, eps);
	print_quad_eq_(a, c, b, eps);
	print_quad_eq_(b, a, c, eps);
	print_quad_eq_(b, c, a, eps);
	print_quad_eq_(c, a, b, eps);
	print_quad_eq_(c, b, a, eps);
	return NO_EXCEPTION;
}

error_t cmd_divisible(int argc, char** argv) {
	if (argc != 4) {
		fprintf(stderr, "Invalid arguments. See usage for more info.\n");
		return NO_EXCEPTION;
	}

	error_t error;
	long a, b;

	if (FAILED((error = str_to_long(argv[2], &a)))) {
		fprintf(stderr, "Invalid `a`: %s.\n", error.message);
		return NO_EXCEPTION;
	}
	if (FAILED((error = str_to_long(argv[3], &b)))) {
		fprintf(stderr, "Invalid `b`: %s.\n", error.message);
		return NO_EXCEPTION;
	}
	if (a == 0 || b == 0) {
		fprintf(stderr, "Invalid `a` or `b`: one is zero.\n");
		return NO_EXCEPTION;
	}

	fprintf(stdout, "%ld is %s by %ld\n", a, a % b ? "not divisible" : "divisible", b);
	return NO_EXCEPTION;
}

error_t cmd_right_triangle(int argc, char** argv) {
	// <prog> <flag> <eps> <a> <b> <c>
	if (argc != 6) {
		fprintf(stderr, "Invalid arguments. See usage for more info.\n");
		return NO_EXCEPTION;
	}

	error_t error;
	double eps;
	double a, b, c;

	if (FAILED((error = str_to_double(argv[2], &eps)))) {
		fprintf(stderr, "Invalid `eps`: %s.\n", error.message);
		return NO_EXCEPTION;
	}
	if (eps <= 0) {
		fprintf(stderr, "Invalid `eps`: may not be less or equal to zero.\n");
		return NO_EXCEPTION;
	}
	if (FAILED((error = str_to_double(argv[3], &a)))) {
		fprintf(stderr, "Invalid `a`: %s.\n", error.message);
		return NO_EXCEPTION;
	}
	if (FAILED((error = str_to_double(argv[4], &b)))) {
		fprintf(stderr, "Invalid `b`: %s.\n", error.message);
		return NO_EXCEPTION;
	}
	if (FAILED((error = str_to_double(argv[5], &c)))) {
		fprintf(stderr, "Invalid `c`: %s.\n", error.message);
		return NO_EXCEPTION;
	}

	fprintf(stdout, "%f %f %f %s build a right triangle\n", a, b, c,
	        task_right_triangle(a, b, c, eps) ? "can" : "cannot");
	return NO_EXCEPTION;
}