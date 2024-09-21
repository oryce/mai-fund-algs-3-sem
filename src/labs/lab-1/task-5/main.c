#include <stdio.h>

#include "lib/conv.h"
#include "lib/error.h"
#include "tasks.h"

typedef error_t (*series_func_t)(double eps, double x, double* out);

typedef struct series {
	char name[8];
	series_func_t func;
} series_t;

int main(int argc, char** argv) {
	error_t error;

	if (argc < 3) {
		printf(
		    "Usage: %s <eps> <x>\n"
		    "Computes various series values given an error margin.\n",
		    argv[0]);
		return -ERROR_INVALID_PARAMETER;
	}

	double eps;
	error = str_to_double(argv[1], &eps);
	if (error != ERROR_SUCCESS) {
		fprintf(stderr, "Invalid 'epsilon': malformed number or out of range.\n");
		return -ERROR_INVALID_PARAMETER;
	}

	double x;
	error = str_to_double(argv[2], &x);
	if (error != ERROR_SUCCESS) {
		fprintf(stderr, "Invalid 'x': malformed number or out of range.\n");
		return -ERROR_INVALID_PARAMETER;
	}

	series_t series[] = {
	    {"a)", &compute_series_a},
	    {"b)", &compute_series_b},
	    {"c)", &compute_series_c},
	    {"d)", &compute_series_d},
	};
	int nSeries = sizeof(series) / sizeof(series_t);

	for (int i = 0; i != nSeries; ++i) {
		double value;
		error = series[i].func(eps, x, &value);
		if (error != ERROR_SUCCESS) {
			printf("%8s | cannot be computed\n", series[i].name);
		} else {
			printf("%8s | %f\n", series[i].name, value);
		}
	}

	return ERROR_SUCCESS;
}
