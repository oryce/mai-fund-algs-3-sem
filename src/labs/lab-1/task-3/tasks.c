#include "tasks.h"

#include <math.h>
#include <stdbool.h>

#define ZERO(a, eps) (fabs(a) < eps)
#define EQUAL(a, b, eps) (fabs((a) - (b)) < eps)
#define LESS(a, b, eps) ((a) - (b) < eps)

quad_sol_t task_quad_eq(double a, double b, double c, double eps) {
	quad_sol_t result;

	if (ZERO(a, eps) && ZERO(b, eps) && ZERO(c, eps)) {
		result.type = QUAD_INFINITE_SOLUTIONS;
	} else if ((ZERO(a, eps) && ZERO(b, eps) /* && c != 0 */) || LESS(b * b, 4 * a * c, eps)) {
		result.type = QUAD_NO_SOLUTIONS;
	} else if (EQUAL(b * b, 4 * a * c, eps)) {
		result.type = QUAD_SINGLE_SOLUTION;
		result.solution.single = -b / 2 * a;
	} else if (ZERO(a, eps)) {
		result.type = QUAD_SINGLE_SOLUTION;
		result.solution.single = -c / b;
	} else {
		double sqrt_d = sqrt(b * b - 4 * a * c);
		result.type = QUAD_MULTIPLE_SOLUTIONS;
		result.solution.multiple[0] = (-b - sqrt_d) / (2 * a);
		result.solution.multiple[1] = (-b + sqrt_d) / (2 * a);
	}

	return result;
}

inline bool task_right_triangle(double a, double b, double c, double eps) {
	bool non_zero = a != 0 && b != 0 && c != 0;
	bool triangle = LESS(a, b + c, eps) && LESS(b, a + c, eps) && LESS(c, a + b, eps);
	bool right_triangle =
	    EQUAL(a * a + b * b, c * c, eps) || EQUAL(a * a + c * c, b * b, eps) || EQUAL(b * b + c * c, a * a, eps);
	return non_zero && triangle && right_triangle;
}
