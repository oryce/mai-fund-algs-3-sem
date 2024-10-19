#include <stdio.h>

#include "lib/convert.h"
#include "lib/error.h"
#include "lib/mth.h"
#include "tasks.h"

typedef error_t (*integral_func_t)(double eps, double x, double* out);

typedef struct series {
	char name[8];
	integral_func_t func;
} series_t;

error_t main_(int argc, char** argv) {
	error_t error;

	if (argc != 3) {
		printf(
		    "Usage: %s <eps> <x>\n"
		    "Computes various series values given an error margin.\n",
		    argv[0]);
		return NO_EXCEPTION;
	}

	double eps;
	error = str_to_double(argv[1], &eps);
	if (FAILED(error) || eps <= 0) {
		fprintf(stderr, "Invalid 'eps': malformed number or out of range.\n");
		return NO_EXCEPTION;
	}

	double x;
	error = str_to_double(argv[2], &x);
	if (FAILED(error)) {
		fprintf(stderr, "Invalid 'x': malformed number or out of range.\n");
		return NO_EXCEPTION;
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
		if (error.code == DivergingException) {
			printf("%8s | cannot be computed\n", series[i].name);
		} else if (FAILED(error)) {
			PASS(error);
		} else {
			printf("%8s | %f\n", series[i].name, value);
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
