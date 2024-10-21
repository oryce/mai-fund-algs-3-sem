#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "task.h"

int main(void) {
	double* g = NULL;
	const double a = 1;
	const int n = 5;

	if (!polynomial_shift(0, a, &g, n, -112.0, 7.0, -2.0, 0.0, 3.0, 1.0)) {
		free(g);
		return 1;
	}

	for (size_t i = 0; i != n + 1; ++i) {
		printf("[%zu]: %lf\n", i, g[i]);
	}

	free(g);
}
