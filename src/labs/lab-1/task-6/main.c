#include <stdio.h>

#include "lib/convert.h"
#include "lib/error.h"
#include "lib/mth.h"
#include "tasks.h"

typedef error_t (*integral_func_t)(double eps, double* out);

typedef struct series {
	char name[8];
	integral_func_t func;
} integral_t;

error_t main_(int argc, char** argv) {
	error_t error;

	if (argc != 2) {
		printf(
		    "Usage: %s <eps>\n"
		    "Computes various integrals given an error margin.\n",
		    argv[0]);
		return NO_EXCEPTION;
	}

	double eps;
	error = str_to_double(argv[1], &eps);
	if (FAILED(error) || eps <= 0) {
		fprintf(stderr, "Invalid 'eps': malformed number or out of range.\n");
		return NO_EXCEPTION;
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
		if (error.code == IntegralException) {
			printf("%8s | cannot be computed\n", integrals[i].name);
		} else if (FAILED(error)) {
			PASS(error);
		} else {
			printf("%8s | %f\n", integrals[i].name, value);
		}
	}

	return NO_EXCEPTION;
}

int main(int argc, char** argv) {
	error_t error = main_(argc, argv);
	if (FAILED(error)) {
		error_fmt_t fmt[] = {&mth_error_to_string};
		error_print_ex(error, fmt, sizeof(fmt) / sizeof(fmt[0]));
		return (int)error.code;
	}
}
