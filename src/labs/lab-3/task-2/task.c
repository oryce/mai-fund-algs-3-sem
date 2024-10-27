#include "task.h"

#include <float.h>
#include <math.h>
#include <stdarg.h>

IMPL_VECTOR(vector_vec_t, vector_t, vec, {NULL})

static error_t cleanup(error_t error, vector_vec_t* res, vector_vec_t* vecs) {
	vector_vec_destroy(res);
	vector_vec_destroy(vecs);

	return error;
}

error_t longest_vecs(vector_vec_t* res, norm_t norm, const void* nParam,
                    unsigned n, unsigned nVecs, ...) {
	if (!res || !norm || !nParam || !n || !nVecs) return ERR_INVVAL;

	*res = vector_vec_create();

	// Stores all the vecs passed with their norms.
	vector_vec_t vecs = vector_vec_create();
	// Max norm encountered.
	double maxNorm = 0;
	// Error margin for vectors with the same norm.
	const double eps = 1e-6;

	va_list args;
	va_start(args, nVecs);

	while (nVecs--) {
		vector_t vec = va_arg(args, vector_t);
		double norm_;

		error_t error = norm(&norm_, &vec, n, nParam);
		if (!error) {
			if (isinf(norm_)) error = ERR_OVERFLOW;
			if (isnan(norm_)) error = ERR_UNDERFLOW;
		}
		if (error) {
			va_end(args);
			return cleanup(error, res, &vecs);
		}

		vec.norm = norm_;
		if (!vector_vec_push_back(&vecs, vec)) {
			va_end(args);
			return cleanup(ERR_MEM, res, &vecs);
		}

		maxNorm = fmax(maxNorm, norm_);
	}

	va_end(args);

	for (size_t i = 0; i != vector_vec_size(&vecs); ++i) {
		vector_t* vec = vector_vec_get(&vecs, i);

		if (fabs(vec->norm - maxNorm) < eps) {
			if (!vector_vec_push_back(res, *vec))
				return cleanup(ERR_MEM, res, &vecs);
		}
	}

	vector_vec_destroy(&vecs);
	return 0;
}

error_t norm_1(double* res, const vector_t* vec, unsigned n,
               const void* param) {
	*res = DBL_MIN;

	for (double* x = vec->coords; n--; ++x) {
		*res = fmax(*res, fabs(*x));
	}

	return 0;
}

error_t norm_2(double* res, const vector_t* vec, unsigned n,
               const void* param) {
	double p = *(const double*)param;
	if (p < 1) return ERR_INVVAL;

	double sum = 0;
	for (double* x = vec->coords; n--; ++x) {
		sum += pow(*x, p);
	}

	*res = pow(sum, 1 / p);
	return 0;
}

static double dot_product(unsigned n, const double* a, const double* b) {
	double result = 0;

	for (unsigned i = 0; i != n; ++i) {
		result += a[i] * b[i];
	}

	return result;
}

error_t norm_3(double* res, const vector_t* vec, unsigned n,
               const void* param) {
	const double** A = (const double**)param;

	double* mul = (double*)calloc(n, sizeof(double));
	if (!mul) return ERR_MEM;

	for (int i = 0; i != n; ++i) {
		mul[i] = dot_product(n, A[i], vec->coords);
	}

	*res = sqrt(dot_product(n, mul, vec->coords));
	free(mul);

	return 0;
}
