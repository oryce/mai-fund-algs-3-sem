#include <float.h>
#include <stdlib.h>

#include "task.h"

#define VEC(coords) ((vector_t){(double*)coords})

static void print_vector(unsigned n, vector_t* vec) {
	printf("vector_t[ ");

	for (unsigned k = 0; k != n; ++k) {
		printf("%lf ", vec->coords[k]);
	}

	printf("norm=%lf ]\n", vec->norm);
}

int main(void) {
	const unsigned n = 2;

	const unsigned nNorms = 3;
	// Param to norm 1.
	const double inf = INFINITY;
	// Param to norm 2.
	const double A0[2] = {1, 0};
	const double A1[2] = {0, 1};
	const double* A[2] = {A0, A1};
	// Param to norm 3.
	const double p = 5.0;

	const unsigned nVecs = 3;
	const double vec1[2] = {5, 4};
	const double vec2[2] = {50, 0.01};
	const double vec3[2] = {30, 50};

	vector_t** vecs = (vector_t**)calloc(nNorms, sizeof(vector_t*));
	size_t* lens = (size_t*)calloc(nNorms, sizeof(size_t));

	error_t error =
	    longest_vecs(vecs, lens, n, nNorms, nVecs, &norm_1, &inf, &norm_2, &p,
	                 &norm_3, A, VEC(vec1), VEC(vec2), VEC(vec3));

	if (error) {
		error_print(error);
	} else {
		for (size_t i = 0; i != nNorms; ++i) {
			printf("Norm %zu:\n", i);

			for (size_t j = 0; j != lens[i]; ++j) {
				print_vector(n, &vecs[i][j]);
			}

			printf("\n");
		}
	}

	for (size_t i = 0; i != nNorms; ++i) {
		free(vecs[i]);
	}

	free(vecs);
	free(lens);

	return 0;
}
