#include "task.h"

#define VEC(coords) ((vector_t){(double*)coords})

static void print_vector(unsigned n, vector_t* vec) {
	printf("vector_t[ ");

	for (unsigned k = 0; k != n; ++k) {
		printf("%lf ", vec->coords[k]);
	}

	printf("]\n");
}

int main(void) {
	vector_vec_t res;
	const unsigned n = 2;
	const unsigned nVecs = 3;

	const double A0[2] = {1, 0};
	const double A1[2] = {0, 1};
	const double* A[2] = {A0, A1};

	const double vec1[2] = {5, 4};
	const double vec2[2] = {10, 5};
	const double vec3[2] = {30, 50};

	error_t error = longest_vecs(&res, &norm_2, A, n, nVecs, VEC(vec1),
	                             VEC(vec2), VEC(vec3));
	if (error) {
		error_print(error);
		return error;
	}

	for (size_t i = 0; i != vector_vec_size(&res); ++i) {
		vector_t* vec = vector_vec_get(&res, i);
		print_vector(n, vec);
	}

	return 0;
}
