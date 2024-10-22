#include "tasks.h"

#include <math.h>
#include <stdarg.h>

#include "lib/convert.h"
#include "lib/jtckdint.h"
#include "lib/mth.h"

const double M_2PI = M_PI * 2;

error_t task_convex(bool* result, double eps, size_t n, ...) {
	if (n < 3) return ERR_INVVAL;
	if (result == NULL) return ERR_INVVAL;

	va_list args;

	point_t p0;  // Previous point
	point_t p1;  // Current point
	double d0;   // Previous direction
	double d1;   // Current direction
	double angles = 0;
	double orientation = 0;

	// Advance va_args to find two last points.
	va_start(args, n);
	for (size_t i = 0; i != n - 2; ++i) {
		va_arg(args, point_t);
	}
	p0 = va_arg(args, point_t);
	p1 = va_arg(args, point_t);

	if (p1.x == p0.x && p1.y == p0.y) {
		va_end(args);
		return ERR_INVVAL;
	}

	d1 = atan2(p1.x - p0.x, p1.y - p0.y);

	va_start(args, n);

	for (size_t i = 0; i != n; ++i) {
		p0 = p1;
		d0 = d1;
		p1 = va_arg(args, point_t);

		if (p1.x == p0.x && p1.y == p0.y) {
			va_end(args);
			return ERR_INVVAL;
		}

		d1 = atan2(p1.x - p0.x, p1.y - p0.y);

		double angle = d1 - d0;
		if (angle - (-M_PI) <= eps) angle += M_2PI;
		if (angle - M_PI > eps) angle -= M_2PI;

		if (i == 0) {
			if (angle == 0) {
				*result = false;
				va_end(args);
				return 0;
			}

			orientation = angle /* - 0 */ > eps ? 1 : -1;
		} else {
			if (orientation * angle /* - 0 */ <= eps) {
				*result = false;
				va_end(args);
				return 0;
			}
		}

		angles += angle;
	}

	*result = fabs(round(angles / M_2PI)) == 1;
	va_end(args);
	return 0;
}

error_t task_polynomial(double* result, double x, int n, ...) {
	if (n < 0) return ERR_INVVAL;
	if (result == 0) return ERR_INVVAL;

	va_list args;
	va_start(args, n);

	*result = va_arg(args, double);
	for (size_t i = 1; i != n + 1; ++i) {
		*result = *result * x + va_arg(args, double);
	}

	va_end(args);
	return 0;
}

error_t is_kaprekar_(const char* number, int base, bool* result) {
	if (!number || !result || *number == '-') return ERR_INVVAL;

	long n;
	if (long_from_base(number, strlen(number), base, &n)) return ERR_INVVAL;

	if (n == 0) {
		*result = false;
		return 0;
	}

	long nSq;
	if (ckd_mul(&nSq, n, n)) return ERR_OVERFLOW;

	char nSqStr[65];
	CHECK(long_to_base(nSq, base, nSqStr, sizeof(nSqStr)),
	      "can't convert square to string");

	// Split the squared number and check if the parts add up to `n`.
	size_t length = strlen(nSqStr);

	for (size_t i = 0; i < length; ++i) {
		char leftStr[65];
		char rightStr[65];

		strncpy(leftStr, nSqStr, i);
		leftStr[i] = '\0';
		strncpy(rightStr, nSqStr + i, sizeof(rightStr));

		long left = 0;
		long right = 0;

		if (i != 0)
			CHECK(long_from_base(leftStr, i, base, &left),
			      "can't convert left part to long");
		if (length - i != 0)
			CHECK(long_from_base(rightStr, length - i, base, &right),
			      "can't convert right part to long");

		if (left + right == n) {
			*result = true;
			return 0;
		}
	}

	*result = false;
	return 0;
}

error_t task_kaprekar(vector_ptr_t* out, int base, size_t n, ...) {
	if (out == NULL) return ERR_INVVAL;
	*out = vector_ptr_create();

	va_list args;
	va_start(args, n);

	while (n--) {
		const char* number = va_arg(args, const char*);
		bool kaprekar = false;

		if (is_kaprekar_(number, base, &kaprekar)) {
			vector_ptr_destroy(out);
			va_end(args);
			return ERR_INVVAL;
		}

		if (kaprekar) {
			if (!vector_ptr_push_back(out, (void*)number)) {
				vector_ptr_destroy(out);
				va_end(args);
				return ERR_MEM;
			}
		}
	}

	va_end(args);
	return 0;
}
