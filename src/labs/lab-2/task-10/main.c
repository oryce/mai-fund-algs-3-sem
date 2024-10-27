#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "lib/mth.h"
#include "task.h"

double rand_double(void) { return mth_rand_double(-100, 100); }

double polynomial(double x, double a, const double* f, int degree) {
	if (!f) return 0.0;

	double value = 0;

	for (int i = 0; i != degree + 1; ++i) {
		value += (double)powl(x - a, i) * f[i];
	}

	return value;
}

bool fuzz(void) {
	size_t iterations = 1000;

	while (iterations--) {
		const double f[] = {rand_double(), rand_double(), rand_double(),
		                    rand_double(), rand_double()};
		const int degree = sizeof(f) / sizeof(f[0]) - 1;

		const double a = rand_double();
		double* g;

		if (!polynomial_shift(0, a, &g, degree, f[0], f[1], f[2], f[3], f[4])) {
			free(g);
			return false;
		}

		const double x = rand_double();
		const double p0 = polynomial(x, 0, f, degree);
		const double p1 = polynomial(x, a, g, degree);

		if (fabs(p0 - p1) > 1e6) {
			printf(
			    "it no workie (break here): \n"
			    "  p0 = %lf\n"
			    "  p1 = %lf\n",
			    p0, p1);

			free(g);
			return false;
		}

		free(g);
	}

	return true;
}

int main(void) {
	srand(time(NULL));  // NOLINT(*-msc51-cpp)
	return fuzz() ? 0 : 1;
}
