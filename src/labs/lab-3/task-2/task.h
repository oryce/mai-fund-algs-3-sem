#include "lib/collections/vector.h"
#include "lib/error.h"

typedef struct vector {
	double* coords;
	double norm;
} vector_t;

DEFINE_VECTOR(vector_vec_t, vector_t, vec)

typedef error_t (*norm_t)(double* res, const vector_t* vec, unsigned n,
                          const void* param);

error_t norm_1(double* res, const vector_t* vec, unsigned n, const void* param);

error_t norm_2(double* res, const vector_t* vec, unsigned n, const void* param);

error_t norm_3(double* res, const vector_t* vec, unsigned n, const void* param);

error_t longest_vecs(vector_vec_t* res, norm_t norm, const void* nParam,
                    unsigned n, unsigned nVecs, ...);
