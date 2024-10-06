#include <stdio.h>

#include "lib/convert.h"
#include "lib/error.h"
#include "tasks.h"

typedef error_t (*integral_func_t)(double eps, double* out);

typedef struct series {
	char name[8];
	integral_func_t func;
} integral_t;

int main(int argc, char** argv) {
	error_t error;

	if (argc != 2) {
		printf(
		    "Usage: %s <eps>\n"
		    "Computes various integrals given an error margin.\n",
		    argv[0]);
		return -ERROR_INVALID_PARAMETER;
	}

	double eps;
	error = str_to_double(argv[1], &eps);
	if (error != ERROR_SUCCESS || eps <= 0) {
		fprintf(stderr, "Invalid 'eps': malformed number or out of range.\n");
		return -ERROR_INVALID_PARAMETER;
	}

	integral_t integrals[] = {
	    {"a)", &compute_integral_a},
	    {"b)", &compute_integral_b},
	    {"c)", &compute_integral_c},
	    {"d)", &compute_integral_d},
	};
	int nIntegrals = sizeof(integrals) / sizeof(integral_t);

	for (int i = 0; i != nIntegrals; ++i) {
		double value;
		error = integrals[i].func(eps, &value);
		if (error != ERROR_SUCCESS) {
			printf("%8s | cannot be computed\n", integrals[i].name);
		} else {
			printf("%8s | %f\n", integrals[i].name, value);
		}
	}

	return ERROR_SUCCESS;
}
