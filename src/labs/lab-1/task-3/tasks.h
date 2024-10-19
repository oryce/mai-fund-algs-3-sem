#include "lib/error.h"

typedef enum quad_sol_type {
	QUAD_INFINITE_SOLUTIONS,
	QUAD_NO_SOLUTIONS,
	QUAD_SINGLE_SOLUTION,
	QUAD_MULTIPLE_SOLUTIONS
} quad_sol_type_t;

typedef struct quad_sol {
	quad_sol_type_t type;
	union {
		double single;
		double multiple[2];
	} solution;
} quad_sol_t;

quad_sol_t task_quad_eq(double a, double b, double c, double eps);

bool task_right_triangle(double a, double b, double c, double eps);
