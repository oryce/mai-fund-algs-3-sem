#include "task.h"

#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

typedef struct norm_data {
	norm_t func;
	void* param;
} norm_data_t;

static error_t longest_vecs0(vector_t* inVecs, unsigned nVecs, unsigned n,
                             norm_t norm, const void* nParam,
                             vector_t* outVectors, size_t* outLength) {
	double maxNorm = 0;       // Max norm encountered.
	const double eps = 1e-6;  // Error margin for vectors with the same norm.

	for (size_t i = 0; i != nVecs; ++i) {
		vector_t* vec = &inVecs[i];
		double norm_;

		error_t error = norm(&norm_, vec, n, nParam);
		if (!error) {
			if (isinf(norm_)) error = ERR_OVERFLOW;
			if (isnan(norm_)) error = ERR_UNDERFLOW;
		}
		if (error) return error;

		vec->norm = norm_;
		maxNorm = fmax(maxNorm, norm_);
	}

	for (size_t i = 0; i != nVecs; ++i) {
		vector_t vec = inVecs[i];

		if (fabs(vec.norm - maxNorm) < eps) {
			outVectors[*outLength] = vec;
			*outLength += 1;
		}
	}

	return 0;
}

static error_t cleanup(error_t error, vector_t* vecs, norm_data_t* norms) {
	free(vecs);
	free(norms);

	return error;
}

error_t longest_vecs(vector_t** outVecs, size_t* outLens, unsigned n,
                     unsigned nNorms, unsigned nVecs, ...) {
	if (!outVecs || !outLens) return ERR_INVVAL;

	vector_t* vecs = NULL;
	norm_data_t* norms = NULL;

	vecs = (vector_t*)calloc(nVecs, sizeof(vector_t));
	if (!vecs) return cleanup(ERR_MEM, vecs, norms);
	norms = (norm_data_t*)calloc(nNorms, sizeof(norm_data_t));
	if (!norms) return cleanup(ERR_MEM, vecs, norms);

	va_list args;
	va_start(args, nVecs);

	for (size_t i = 0; i != nNorms; ++i) {
		norms[i] = (norm_data_t){.func = va_arg(args, norm_t),
		                         .param = va_arg(args, void*)};
	}

	for (size_t i = 0; i != nVecs; ++i) {
		vecs[i] = va_arg(args, vector_t);
	}

	va_end(args);

	for (size_t i = 0; i != nNorms; ++i) {
		outVecs[i] = calloc(nVecs, sizeof(vector_t));
		outLens[i] = 0;
		if (!outVecs[i]) return cleanup(ERR_MEM, vecs, norms);

		error_t error = longest_vecs0(vecs, nVecs, n, norms[i].func,
		                              norms[i].param, outVecs[i], &outLens[i]);
		if (error) return cleanup(error, vecs, norms);
	}

	return cleanup(0, vecs, norms);
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
