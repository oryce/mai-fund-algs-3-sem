#pragma once

#include <stdbool.h>

#include "lib/error.h"
#include "lib/collections/vector.h"

typedef struct point {
	double x;
	double y;
} point_t;

error_t task_convex(bool* result, double eps, size_t n, ...);

error_t task_polynomial(double* result, double x, int n, ...);

error_t task_kaprekar(vector_ptr_t* out, int base, size_t n, ...);
